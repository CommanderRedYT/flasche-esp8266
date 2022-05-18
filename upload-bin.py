#!/usr/bin/env python3
# upload file path specified via command line
# example: upload-bin.py --file /home/user/file.txt
# then, ask the user for the server password and send file to https://lamp.bobbycar.cloud/upload-firmware

import argparse
import requests
import sys
import os
from getpass import getpass


def upload_file(file_path, password):
    """
    Uploads a file to the server.
    :param file_path: path to the file to upload
    :return:
    """
    if not os.path.isfile(file_path):
        file_path = os.path.join(os.getcwd(), '.pio/build/esp8266dev/firmware.bin')

    print('Uploading file: {}'.format(file_path))

    # get git commit hash
    commit_hash = os.popen('git rev-parse HEAD').read().strip()
    print(commit_hash)

    # get the server password
    if not password or password == "":
        password = getpass('Server password: ')

    url = "https://lamps.bobbycar.cloud/upload-firmware"
    files = {'firmware': open(file_path, 'rb')}
    r = requests.post(url, files=files, headers={'Authorization': password, 'X-Git-Commit-Hash': commit_hash})
    print(r.text)


def main():
    parser = argparse.ArgumentParser(description='Uploads a file to the server.')
    parser.add_argument('--file', type=str, help='path to the file to upload')
    parser.add_argument('--password', type=str, help='server password')
    args = parser.parse_args()
    upload_file(args.file or "", args.password or "")


if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        print()
        sys.exit(0)
