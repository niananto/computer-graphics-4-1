import glob
import zipfile
import json
import subprocess
import os
import shutil
import sys
import numpy as np


EPS = 1e-6
SUBMISSION_DIR = 'submissions'
TEST_DIR = 'test_cases'
# CONFIG = json.load(open('config.json'))

test_scene_files = glob.glob(f'{TEST_DIR}/*/scene.txt')
n_test = len(test_scene_files)
submissions = glob.glob(f'{SUBMISSION_DIR}/*.zip')


def file_to_floats(path):
    floats = []
    with open(path) as file:
        for line in file:
            line = line.strip()
            if len(line) == 0: continue
            line = map(float, line.split())
            floats.extend(line)
    return floats


def compare_file(correct, output):
    correct_floats = np.array(file_to_floats(correct))
    ouput_floats = np.array(file_to_floats(output))
    difference = np.abs(correct_floats - ouput_floats)
    mismatch = (difference > EPS).astype(np.int8)
    return mismatch.sum()

for submission in submissions:
    roll = submission[-11:-4]

    print(f'[{roll}] Unzipping submission')
    with zipfile.ZipFile(submission, 'r') as zip:
        zip.extractall(f'{SUBMISSION_DIR}')

    print(f'[{roll}] Compiling submission')
    subprocess.run([
        'g++', 
        os.path.join(SUBMISSION_DIR, roll, 'main.cpp'), 
        '-o',
        os.path.join(SUBMISSION_DIR, roll, 'main')
    ])

    for test in range(1, n_test + 1):
        scene_file_main = os.path.join(TEST_DIR, str(test), 'scene.txt')
        scene_file = os.path.join(SUBMISSION_DIR, roll, 'scene.txt')
        shutil.copy2(scene_file_main, scene_file)

        os.chdir(os.path.join(SUBMISSION_DIR, roll))
        subprocess.run([
            'main.exe'
        ])
        # moving up two directories
        os.chdir('..')
        os.chdir('..')

        passed = 0

        for stage in range(1, 4):
            correct_file = os.path.join(TEST_DIR, str(test), f'stage{stage}.txt')
            output_file = os.path.join(SUBMISSION_DIR, roll, f'stage{stage}.txt')
            mismatch = compare_file(correct_file, output_file)
            if(mismatch > 1):
                print(f'[{roll}] Mismatch at stage{stage} of test {test}')
            else:
                passed += 1
        
        if passed == 3:
            print(f'[{roll}] Test {test} okay!')

    # subprocess.run(["ls", "-l"]) 