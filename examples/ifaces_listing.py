import csv
import re
import subprocess
import sys

command_listing: str = "\"../cmake-build-release/wifi\" ifaces list -gd"

# wifi iface "{...}" profiles list

try:
    raw_wifi_info: bytes = subprocess.check_output(command_listing, shell=True, stderr=None)
except subprocess.CalledProcessError as listing_err:
    print("Error: {}".format(listing_err))
    sys.exit(1)

for row in raw_wifi_info.decode("utf-8").split('\n'):
    # Ignoring empty lines
    if len(row.strip()) == 0:
        continue
    
    # Excluding debugging prints  (Not required on releases)
    if re.match("^[DET]\\>", row):
        continue
    
    # Printing the actual data
    row_data = list(csv.reader([row], delimiter=';'))[0]
    print(f"GUID: {row_data[0]}, Description: {row_data[1]}")
