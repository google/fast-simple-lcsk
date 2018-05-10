# Copyright 2018 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import subprocess
import csv

filenames = [
    'Homo_sapiens.GRCh38.dna.chromosome.1.fa',
    'Homo_sapiens.GRCh38.dna.chromosome.2.fa',
    'Homo_sapiens.GRCh38.dna.chromosome.3.fa',
]

log_file = 'ratios_hg.csv'

def get_done():
    done = set()
    with open(log_file) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            done.add((int(row['K']), row['FILENAME']))
    return done


def dump_result(new_row):
    rows = []
    fieldnames = None
    with open(log_file) as csvfile:
        reader = csv.DictReader(csvfile)
        fieldnames = reader.fieldnames
        for row in reader:
            rows.append(row)
    rows.append(new_row)

    with open(log_file, 'w') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for row in rows:
            writer.writerow(row)

done = get_done()

for k in reversed(range(2, 31)):
    for filename in filenames:
        print ('k = {}, filename = {}'.format(k, filename))
        if (k, filename) in done:
            print ('Done before')
        else:
            x = subprocess.check_output("./stats_fasta {} {}".format(k, filename), shell=True)
            n, lcskpp, mp_created, mp_max_alive = x.strip().split(' ')
            dump_result({
                'N': n,
                'K': k,
                'LCSK': lcskpp,
                'MP_CREATED': mp_created,
                'MP_MAX_ALIVE': mp_max_alive,
                'FILENAME': filename
            })
            print ('Done')
