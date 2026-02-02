#!/usr/bin/env python3

import re

ld_file_path = 'firmware/application/external/external.ld'

with open(ld_file_path, 'r') as f:
    content = f.read()

# Parse current regions
regions = []
pattern = r'ram_external_app_(\w+)\s*\(rwx\)\s*:\s*org\s*=\s*(0x[A-Fa-f0-9]+),\s*len\s*=\s*(\d+)k'
matches = re.finditer(pattern, content)

for match in matches:
    app_name = match.group(1)
    address = int(match.group(2), 16)
    length = int(match.group(3)) * 1024
    regions.append({
        'app_name': app_name,
        'address': address,
        'length': length
    })

regions.sort(key=lambda x: x['address'])

# Calculate new addresses for 64KB spacing
current_address = 0xADB10000
step = 0x10000  # 64KB

print(f'Fixing addresses for {len(regions)} external apps with 64KB spacing:')
for i, region in enumerate(regions):
    old_address = hex(region['address'])
    new_address = hex(current_address)
    print(f'  {i+1:2d}. {region["app_name"]:25s} | {old_address:10s} -> {new_address:10s}')
    
    # Update the content - need to handle different hex formats
    # Try uppercase hex
    old_pattern_upper = f'org = 0x{region["address"]:08X}, len = 32k'
    new_pattern = f'org = {new_address}, len = 32k'
    if old_pattern_upper in content:
        content = content.replace(old_pattern_upper, new_pattern)
    else:
        # Try lowercase hex
        old_pattern_lower = f'org = 0x{region["address"]:08x}, len = 32k'
        if old_pattern_lower in content:
            content = content.replace(old_pattern_lower, new_pattern)
    
    current_address += step

# Write back
with open(ld_file_path, 'w') as f:
    f.write(content)

print(f'\nUpdated {ld_file_path}')
