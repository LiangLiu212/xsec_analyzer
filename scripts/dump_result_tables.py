#!/usr/bin/env python3
"""Dump numeric baseline tables from an UnfoldedCrossSection.root file.

For every units directory (XsecUnits, EventCountUnits) and every slice
subdirectory (binnumber, p_mu, cos_theta, ...) it writes one markdown table:
bin | central value | fractional error per uncertainty source. The central
value is common to all per-source histograms; each histogram's bin errors
carry that source's uncertainty.

Also dumps any standalone TH1 objects at the units level (e.g. TotalBNBData,
TotalBackground) and the diagonal of the Covariances/total matrix if present.
"""

import argparse

import numpy as np
import uproot


def main():
    ap = argparse.ArgumentParser(
        description="Dump numeric tables from UnfoldedCrossSection.root."
    )
    ap.add_argument("--input", required=True,
                    help="UnfoldedCrossSection.root path")
    ap.add_argument("--report", default=None,
                    help="write markdown to this path (default: stdout only)")
    ap.add_argument("--sources", default=None,
                    help="comma-separated uncertainty sources to tabulate "
                         "(default: all found)")
    args = ap.parse_args()

    want = args.sources.split(",") if args.sources else None
    lines = [f"# Result tables: {args.input}", ""]

    with uproot.open(args.input) as f:
        units_dirs = [k.split(";")[0] for k, c in f.classnames().items()
                      if c == "TDirectory" or c.startswith("TDirectory")]
        for units in units_dirs:
            udir = f[units]
            slice_dirs, standalone = [], []
            for k, c in udir.classnames().items():
                name = k.split(";")[0]
                if c.startswith("TDirectory"):
                    slice_dirs.append(name)
                elif c.startswith("TH1"):
                    standalone.append(name)

            for sl in sorted(slice_dirs):
                if sl == "Covariances":
                    cov_dir = udir[sl]
                    if "total" in {k.split(";")[0] for k in cov_dir.keys()}:
                        m = np.asarray(cov_dir["total"].member("fElements"))
                        n = int(np.sqrt(m.size))
                        diag = np.sqrt(np.diag(m.reshape(n, n)))
                        lines.append(f"## {units}/Covariances/total — "
                                     f"sqrt(diagonal), {n} bins")
                        lines.append("")
                        lines.append("| bin | sigma |")
                        lines.append("|---|---|")
                        lines.extend(f"| {i} | {v:.6g} |"
                                     for i, v in enumerate(diag))
                        lines.append("")
                    continue

                sdir = udir[sl]
                hists = {}
                for k, c in sdir.classnames().items():
                    name = k.split(";")[0]
                    if not c.startswith("TH1"):
                        continue
                    source = name[len(sl) + 1:] if name.startswith(sl + "_") \
                        else name
                    if want and source not in want:
                        continue
                    h = sdir[name]
                    hists[source] = (h.values(), h.errors())
                if not hists:
                    continue
                sources = sorted(hists)
                cv = next(iter(hists.values()))[0]
                lines.append(f"## {units}/{sl} — CV and fractional errors")
                lines.append("")
                lines.append("| bin | CV | " + " | ".join(sources) + " |")
                lines.append("|---" * (len(sources) + 2) + "|")
                for i in range(len(cv)):
                    fracs = []
                    for s in sources:
                        v, e = hists[s]
                        fracs.append(f"{e[i] / v[i]:.4f}" if v[i] != 0
                                     else "-")
                    lines.append(f"| {i} | {cv[i]:.6g} | "
                                 + " | ".join(fracs) + " |")
                lines.append("")

            for name in sorted(standalone):
                h = udir[name]
                v, e = h.values(), h.errors()
                lines.append(f"## {units}/{name}")
                lines.append("")
                lines.append("| bin | content | error |")
                lines.append("|---|---|---|")
                lines.extend(f"| {i} | {v[i]:.6g} | {e[i]:.4g} |"
                             for i in range(len(v)))
                lines.append("")

    text = "\n".join(lines)
    print(text)
    if args.report:
        with open(args.report, "w") as fh:
            fh.write(text + "\n")


if __name__ == "__main__":
    main()
