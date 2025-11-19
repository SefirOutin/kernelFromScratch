#!/usr/bin/env python3
import subprocess
import sys

def generate_symbol_table(elf_file):
    # Obtenir les symboles avec nm
    result = subprocess.run(
        ['nm', '-n', elf_file],
        capture_output=True,
        text=True
    )
    
    symbols = []
    for line in result.stdout.strip().split('\n'):
        parts = line.split()
        if len(parts) >= 3 and parts[1] in ['T', 't']:  # Symboles de code
            addr = parts[0]
            name = parts[2]
            symbols.append((addr, name))
    
    # Générer le fichier C
    print('#include "symbols.h"')
    print()
    print('struct symbol_entry {')
    print('    unsigned int addr;')
    print('    const char *name;')
    print('};')
    print()
    print('static struct symbol_entry symbol_table[] = {')
    
    for addr, name in symbols:
        print(f'    {{0x{addr}, "{name}"}},')
    
    print('    {0, NULL}')
    print('};')
    print()

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <elf_file>")
        sys.exit(1)
    
    generate_symbol_table(sys.argv[1])
