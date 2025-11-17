#!/usr/bin/env python3
import subprocess
import sys
import re

def addr2line(elf_file, addr):
    result = subprocess.run(
        ['addr2line', '-e', elf_file, '-f', '-s', addr],
        capture_output=True,
        text=True
    )
    lines = result.stdout.strip().split('\n')
    if len(lines) >= 2:
        return f"{lines[0]} at {lines[1]}"
    return "unknown"

# Lire l'entrée ligne par ligne
for line in sys.stdin:
    print(line, end='')
    
    # Chercher les adresses
    match = re.search(r'\[(\d+)\] (0x[0-9a-f]+)', line)
    if match:
        addr = match.group(2)
        symbol = addr2line('kernel.elf', addr)
        print(f"    └─> {symbol}")