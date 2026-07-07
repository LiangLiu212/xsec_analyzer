#!/usr/bin/env python3
"""Content-level comparison of two Universes.root files.

Subdirectory names under the response-matrix TDirectory embed the absolute
paths of the input ntuples ('+'-separated), so two builds of the same physics
content from different machines never match by full key name. This tool
matches subdirectories by the trailing ntuple basename (e.g.
'xsec-ana-r1_bnb.root') and compares every histogram bin-by-bin.

'total_*' subdirectories are excluded by default: they are a normalization
cache keyed by the file-properties config *name* (not content) and are
expected to differ across setups.

Never compare ROOT files by md5 — they embed timestamps.
"""

import argparse
import sys

import numpy as np
import uproot


def tail_key(path_component):
    """Reduce a '+'-separated embedded path to its trailing basename."""
    return path_component.split("+")[-1]


def collect_histograms(filename, include_total=False):
    """Map (subdir_tail, hist_name) -> bin-content array (flow included)."""
    out = {}
    with uproot.open(filename) as f:
        for name, cls in f.classnames(recursive=True).items():
            if not cls.startswith(("TH1", "TH2", "TH3")):
                continue
            clean = name.split(";")[0]
            parts = clean.split("/")
            hist = parts[-1]
            subdir = "/".join(parts[:-1])
            tail = "/".join(tail_key(p) for p in parts[:-1]) if subdir else ""
            if not include_total and any(
                p.startswith("total_") for p in parts[:-1]
            ):
                continue
            key = (tail, hist)
            if key in out:
                # duplicate after tail-reduction: keep first, flag later
                continue
            out[key] = f[clean].values(flow=True)
    return out


def main():
    ap = argparse.ArgumentParser(
        description="Content-level comparison of two Universes.root files."
    )
    ap.add_argument("--file-a", required=True, help="first ROOT file")
    ap.add_argument("--file-b", required=True, help="second ROOT file")
    ap.add_argument("--report", default=None,
                    help="write markdown report to this path (default: stdout only)")
    ap.add_argument("--include-total", action="store_true",
                    help="also compare 'total_*' normalization-cache dirs")
    ap.add_argument("--fail-on-diff", action="store_true",
                    help="exit 1 if any histogram differs (for determinism gates)")
    ap.add_argument("--max-rows", type=int, default=50,
                    help="max differing histograms to list in the report")
    args = ap.parse_args()

    ha = collect_histograms(args.file_a, args.include_total)
    hb = collect_histograms(args.file_b, args.include_total)

    keys_a, keys_b = set(ha), set(hb)
    common = sorted(keys_a & keys_b)
    only_a = sorted(keys_a - keys_b)
    only_b = sorted(keys_b - keys_a)

    identical, differing = [], []
    for key in common:
        a, b = ha[key], hb[key]
        if a.shape != b.shape:
            differing.append((key, np.inf, a.sum(), b.sum(), "shape"))
            continue
        if np.array_equal(a, b):
            identical.append(key)
            continue
        denom = np.where(b != 0, np.abs(b), 1.0)
        maxrel = float(np.max(np.abs(a - b) / denom))
        differing.append((key, maxrel, float(a.sum()), float(b.sum()), ""))

    differing.sort(key=lambda r: -r[1])

    lines = []
    lines.append(f"# compare_universes: {args.file_a} vs {args.file_b}")
    lines.append("")
    lines.append(f"- histograms in A: {len(ha)}, in B: {len(hb)}")
    lines.append(f"- matched (tail-keyed): {len(common)}  |  only in A: "
                 f"{len(only_a)}  |  only in B: {len(only_b)}")
    lines.append(f"- **identical: {len(identical)}**  |  **differing: "
                 f"{len(differing)}**")
    lines.append("")
    if differing:
        lines.append("| subdir/hist | max rel diff | sum A | sum B |")
        lines.append("|---|---|---|---|")
        for key, maxrel, sa, sb, note in differing[: args.max_rows]:
            tag = f"{key[0]}/{key[1]}" + (f" ({note})" if note else "")
            lines.append(f"| {tag} | {maxrel:.3e} | {sa:.6g} | {sb:.6g} |")
        if len(differing) > args.max_rows:
            lines.append(f"| … {len(differing) - args.max_rows} more … | | | |")
        lines.append("")
    for label, keys in (("only in A", only_a), ("only in B", only_b)):
        if keys:
            lines.append(f"<details><summary>{label} ({len(keys)})</summary>")
            lines.extend(f"- {k[0]}/{k[1]}" for k in keys[:30])
            if len(keys) > 30:
                lines.append(f"- … {len(keys) - 30} more")
            lines.append("</details>")
            lines.append("")

    text = "\n".join(lines)
    print(text)
    if args.report:
        with open(args.report, "w") as fh:
            fh.write(text + "\n")

    if args.fail_on_diff and differing:
        sys.exit(1)


if __name__ == "__main__":
    main()
