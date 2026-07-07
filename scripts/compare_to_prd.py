#!/usr/bin/env python3
"""Compare a fresh NC1p unfolding against the published-result container
PRD_NC1PXSEC_PRD_2025.root and the legacy histos_r0 reference.

Establishes, with exact tests where possible:
  P1  PRD smearcept          == legacy h2_block_signal / h_gen_block_all
  P2  PRD prior_true_signal  == legacy h_true_block_signal
  P3  PRD efficiency_matrix  == diag(column sums of smearcept)
  F1  fresh prior (dagostini blocks) vs legacy h_gen_block_all
  F2  legacy/fresh selected-signal ratio per true bin (MC)
  F3  fresh unfolded (EventCountUnits CV) vs PRD unfolded
      (unfolding_matrix @ data_signal), absolute and shape-normalized
  F4  fresh DataStats fractional errors vs PRD h_frac_stat
  F5  status of PRD h_xsec / h_covmat vs fresh XsecUnits total

All ratios/diffs are printed as markdown tables for the validation note.
"""

import argparse

import numpy as np
import uproot


def mat(f, key):
    m = f[key]
    return np.array(m.member("fElements"), dtype=float).reshape(
        m.member("fNrows"), m.member("fNcols"))


def block_vec(dag, name):
    return np.concatenate([mat(dag, f"block0_{name}").ravel(),
                           mat(dag, f"block1_{name}").ravel()])


def table(rows, header):
    out = ["| " + " | ".join(header) + " |",
           "|" + "---|" * len(header)]
    out += ["| " + " | ".join(str(c) for c in r) + " |" for r in rows]
    return "\n".join(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--prd", required=True, help="PRD container ROOT file")
    ap.add_argument("--legacy", required=True,
                    help="legacy histos_r0 reference ROOT file")
    ap.add_argument("--unfolded", required=True,
                    help="fresh UnfoldedCrossSection.root")
    ap.add_argument("--dagostini", required=True,
                    help="fresh dagostini_unfolding_components.root")
    ap.add_argument("--report", default=None, help="markdown output path")
    args = ap.parse_args()

    prd = uproot.open(args.prd)
    leg = uproot.open(args.legacy)
    unf = uproot.open(args.unfolded)
    dag = uproot.open(args.dagostini)

    L = []

    # --- legacy ingredients ---
    h2 = leg["h2_block_signal"].values()
    gen = leg["h_gen_block_all"].values()
    ts = leg["h_true_block_signal"].values()

    # P1: smearcept provenance (file-native orientation)
    sc_prd = mat(prd, "smearcept")
    sc_leg = h2 / np.where(gen > 0, gen, 1)[None, :]
    L.append(f"**P1** PRD smearcept vs legacy h2_block_signal/h_gen_block_all: "
             f"max abs diff = {np.max(np.abs(sc_prd - sc_leg)):.3e}")

    # P2: prior provenance
    pts = mat(prd, "prior_true_signal").ravel()
    L.append(f"**P2** PRD prior_true_signal vs legacy h_true_block_signal: "
             f"max abs diff = {np.max(np.abs(pts - ts)):.3e}")

    # P3: efficiency = column sums of smearcept
    eff_prd = np.diag(mat(prd, "efficiency_matrix"))
    L.append(f"**P3** PRD efficiency diag vs colsum(smearcept): "
             f"max abs diff = {np.max(np.abs(eff_prd - sc_prd.sum(axis=0))):.3e}")

    # F1: fresh prior vs legacy generated
    prior_fresh = block_vec(dag, "prior_true_signal")
    r1 = prior_fresh / np.where(gen > 0, gen, 1)
    L.append(f"**F1** fresh prior / legacy h_gen_block_all: "
             f"min={r1.min():.4f} max={r1.max():.4f} "
             f"(1.0 = exact signal-def + POT match)")

    # F2: selected-signal ratio
    eff_fresh = block_vec(dag, "efficiency")
    sel_fresh = eff_fresh * gen
    r2 = np.where(sel_fresh > 0, ts / sel_fresh, np.nan)
    L.append("\n**F2** legacy/fresh selected-signal ratio per true bin (MC):\n")
    L.append(table([[i, f"{ts[i]:.1f}", f"{sel_fresh[i]:.1f}", f"{r2[i]:.3f}"]
                    for i in range(len(ts))],
                   ["true bin", "legacy sel", "fresh sel", "ratio"]))
    L.append(f"\nmean ratio (bins 1-15) = {np.nanmean(r2[1:]):.3f}, "
             f"rms = {np.nanstd(r2[1:]):.3f}")

    # F3: unfolded comparison
    um = mat(prd, "unfolding_matrix")
    ds = mat(prd, "data_signal").ravel()
    u_prd = um @ ds
    h = unf["EventCountUnits/binnumber/binnumber_DataStats"]
    u_fresh = h.values()
    shp_prd = u_prd / u_prd.sum()
    shp_fresh = u_fresh / u_fresh.sum()
    rows = [[i, f"{u_fresh[i]:.1f}", f"{u_prd[i]:.1f}",
             f"{u_fresh[i]/u_prd[i]:.3f}" if u_prd[i] else "-",
             f"{shp_fresh[i]/shp_prd[i]:.3f}" if shp_prd[i] else "-"]
            for i in range(len(u_fresh))]
    L.append("\n**F3** unfolded signal, fresh vs PRD (= unfolding_matrix @ "
             "data_signal):\n")
    L.append(table(rows, ["bin", "fresh", "PRD", "fresh/PRD",
                          "shape ratio"]))

    # F4: stat fractional errors
    fs_prd = prd["h_frac_stat"].values()
    fs_fresh = h.errors() / np.where(u_fresh != 0, u_fresh, 1)
    L.append("\n**F4** fractional stat errors, fresh DataStats vs PRD "
             "h_frac_stat:\n")
    L.append(table([[i, f"{fs_fresh[i]:.4f}", f"{fs_prd[i]:.4f}",
                     f"{fs_fresh[i]/fs_prd[i]:.2f}" if fs_prd[i] else "-"]
                    for i in range(len(fs_prd))],
                   ["bin", "fresh", "PRD", "fresh/PRD"]))

    # F5: h_xsec / h_covmat status
    hx = prd["h_xsec"].values()
    L.append(f"\n**F5** PRD h_xsec: "
             f"{'EMPTY (all zeros - placeholder)' if not hx.any() else 'filled'}. "
             f"PRD h_covmat diag sqrt range: "
             f"{np.sqrt(np.diag(mat(prd,'h_covmat'))).min():.1f}"
             f"-{np.sqrt(np.diag(mat(prd,'h_covmat'))).max():.1f} (event-count "
             f"units); fresh total covariance currently stats+detVar only.")

    text = "\n".join(L)
    print(text)
    if args.report:
        with open(args.report, "w") as fh:
            fh.write(text + "\n")


if __name__ == "__main__":
    main()
