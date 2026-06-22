import re
import subprocess
import urllib.parse
import sys

def get_test_coverage():
    try:
        subprocess.check_call(['cmake', '-S', '.', '-B', 'cmake-build-debug', '-DCMAKE_BUILD_TYPE=Debug', '-DBUILD_TESTING=ON'])
        subprocess.check_call(['cmake', '--build', 'cmake-build-debug'])
        subprocess.check_call(['ctest', '--test-dir', 'cmake-build-debug', '-C', 'Debug'])
        output = subprocess.check_output(['ctest', '--test-dir', 'cmake-build-debug', '-C', 'Debug', '-T', 'Coverage'], text=True)
        for line in output.splitlines():
            if 'Percentage Coverage:' in line:
                return line.split('Percentage Coverage:')[1].strip()
    except Exception as e:
        print(e)
    return "0.00%"

def get_doc_coverage():
    with open('c89stringutils/c89stringutils_string_extras.h', 'r') as f:
        content = f.read()
    parts = content.split('extern C89STRINGUTILS_EXPORT')
    total = len(parts) - 1
    if total == 0: return "100.00%"
    documented = 0
    for i in range(total):
        text_before = parts[i]
        last_comment_end = text_before.rfind('*/')
        if last_comment_end != -1:
            last_comment_start = text_before.rfind('/**', 0, last_comment_end)
            if last_comment_start != -1:
                documented += 1
    return f"{(documented/total)*100:.2f}%"

def update_readme():
    test_cov = get_test_coverage()
    doc_cov = get_doc_coverage()

    test_cov_enc = urllib.parse.quote(test_cov)
    doc_cov_enc = urllib.parse.quote(doc_cov)

    test_shield = f"![Test Coverage](https://img.shields.io/badge/test__coverage-{test_cov_enc}-brightgreen)"
    doc_shield = f"![Doc Coverage](https://img.shields.io/badge/doc__coverage-{doc_cov_enc}-brightgreen)"

    with open('README.md', 'r') as f:
        readme = f.read()

    # Replace old shields if they exist
    readme = re.sub(r'!\[Test Coverage\]\(https://img\.shields\.io/badge/test__coverage-[^)]+\)\n?', '', readme)
    readme = re.sub(r'!\[Doc Coverage\]\(https://img\.shields\.io/badge/doc__coverage-[^)]+\)\n?', '', readme)

    # Insert new shields next to the license shield
    license_shield = r'(\[!\[License\]\(https://img\.shields\.io/badge/license-[^)]+\)\]\([^)]+\))'

    new_shields = f"\\1\n{doc_shield}\n{test_shield}"

    new_readme = re.sub(license_shield, new_shields, readme, count=1)

    with open('README.md', 'w') as f:
        f.write(new_readme)

if __name__ == '__main__':
    update_readme()
