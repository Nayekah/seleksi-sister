#!/usr/bin/env sage
# -*- coding: utf-8 -*-

from sage.all import Matrix, vector, ZZ, sage_eval

with open("enc.txt", 'r') as f:
    lines = f.readlines()

v = [vector(ZZ, sage_eval(line.strip())) for line in lines if line.strip()]

pubkey = Matrix(ZZ, [
    [47, -77, -85],
    [-49, 78, 50],
    [57, -78, 99]
])

pubkey_inv = pubkey.inverse()

E0 = v[0]
delta_ords = [0]

for i in range(1, len(v)):
    Ei = v[i]
    v_diff = Ei - E0
    
    m_diff_rational = v_diff * pubkey_inv
    m_diff = [round(c) for c in m_diff_rational]
    
    delta_ords.append(m_diff[0])

min_delta = min(delta_ords)
max_delta = max(delta_ords)

lower_bound = 32 - min_delta
upper_bound = 126 - max_delta

for o0_candidate in range(lower_bound, upper_bound + 1):
    flag_ords = [o0_candidate + d for d in delta_ords]
    if all(32 <= o <= 126 for o in flag_ords):
        flag = "".join(chr(o) for o in flag_ords)
        if "HTB{" in flag:
            print(f"Found: {flag}")
            break