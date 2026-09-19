import os
import re
import subprocess
import sys
import urllib.parse


def get_color(pct_str):
    try:
        pct = float(pct_str.rstrip("%"))
        if pct >= 90:
            return "brightgreen"
        if pct >= 80:
            return "green"
        if pct >= 70:
            return "yellowgreen"
        if pct >= 60:
            return "yellow"
        return "red"
    except Exception:
        return "brightgreen"


def get_test_coverage():
    candidate_dirs = ["build_gcc", "cmake-build-debug", "build"]
    build_dir = None

    for d in candidate_dirs:
        if os.path.isdir(d):
            has_cov = False
            for _, _, files in os.walk(d):
                if any(f.endswith(".gcda") or f.endswith(".gcno") for f in files):
                    has_cov = True
                    break
            if has_cov:
                build_dir = d
                break

    if not build_dir:
        build_dir = "cmake-build-debug"
        try:
            subprocess.check_call(
                [
                    "cmake",
                    "-S",
                    ".",
                    "-B",
                    build_dir,
                    "-DCMAKE_BUILD_TYPE=Debug",
                    "-DBUILD_TESTING=ON",
                    "-DCMAKE_C_FLAGS=--coverage",
                    "-DCMAKE_EXE_LINKER_FLAGS=--coverage",
                ]
            )
            subprocess.check_call(["cmake", "--build", build_dir])
            subprocess.check_call(
                [
                    "ctest",
                    "--test-dir",
                    build_dir,
                    "-C",
                    "Debug",
                    "--output-on-failure",
                ]
            )
        except Exception as e:
            print(f"Build failed: {e}", file=sys.stderr)

    if build_dir and os.path.isdir(build_dir):
        # Ensure tests have run and .gcda files exist
        has_gcda = False
        for _, _, files in os.walk(build_dir):
            if any(f.endswith(".gcda") for f in files):
                has_gcda = True
                break
        if not has_gcda:
            try:
                subprocess.check_call(
                    [
                        "ctest",
                        "--test-dir",
                        build_dir,
                        "-C",
                        "Debug",
                        "--output-on-failure",
                    ]
                )
            except Exception as e:
                print(f"ctest run failed: {e}", file=sys.stderr)

        # Ensure CTestCustom.cmake exists in build_dir
        ctest_custom = os.path.join(build_dir, "CTestCustom.cmake")
        if not os.path.exists(ctest_custom):
            with open(ctest_custom, "w") as f:
                f.write(
                    "set(CTEST_CUSTOM_COVERAGE_EXCLUDE \".*tests/.*\" \".*test_.*\" \".*CMakeFiles.*\" \".*greatest.*\")\n"
                )

        # Try ctest -T Coverage
        try:
            output = subprocess.check_output(
                ["ctest", "--test-dir", build_dir, "-C", "Debug", "-T", "Coverage"],
                text=True,
                stderr=subprocess.STDOUT,
            )
            for line in output.splitlines():
                if "Percentage Coverage:" in line:
                    val = line.split("Percentage Coverage:")[1].strip()
                    if val:
                        return val
        except Exception as e:
            print(f"ctest Coverage failed: {e}", file=sys.stderr)

        # Fallback to gcovr if ctest did not return Percentage Coverage
        try:
            res = subprocess.run(
                [
                    "gcovr",
                    "-r",
                    ".",
                    "--print-summary",
                    "-e",
                    ".*tests.*",
                    "-e",
                    ".*CMakeFiles.*",
                    "-e",
                    ".*greatest.*",
                    build_dir,
                ],
                capture_output=True,
                text=True,
            )
            if res.returncode == 0:
                match = re.search(r"lines:\s+([0-9.]+)%", res.stdout)
                if match:
                    return f"{float(match.group(1)):.2f}%"
        except Exception as e:
            print(f"gcovr failed: {e}", file=sys.stderr)

    return "0.00%"


def get_doc_coverage():
    with open("c89stringutils/c89stringutils_string_extras.h", "r") as f:
        content = f.read()
    parts = content.split("extern C89STRINGUTILS_EXPORT")
    total = len(parts) - 1
    if total == 0:
        return "100.00%"
    documented = 0
    for i in range(total):
        text_before = parts[i]
        last_comment_end = text_before.rfind("*/")
        if last_comment_end != -1:
            last_comment_start = text_before.rfind("/**", 0, last_comment_end)
            if last_comment_start != -1:
                documented += 1
    return f"{(documented/total)*100:.2f}%"


def update_readme():
    test_cov = get_test_coverage()
    doc_cov = get_doc_coverage()

    test_color = get_color(test_cov)
    doc_color = get_color(doc_cov)

    test_cov_enc = urllib.parse.quote(test_cov)
    doc_cov_enc = urllib.parse.quote(doc_cov)

    test_shield = f"![Test Coverage](https://img.shields.io/badge/test__coverage-{test_cov_enc}-{test_color})"
    doc_shield = f"![Doc Coverage](https://img.shields.io/badge/doc__coverage-{doc_cov_enc}-{doc_color})"

    with open("README.md", "r") as f:
        readme = f.read()

    # Replace old shields if they exist
    readme = re.sub(
        r"\[?!\[(?:Test|test)[ _]?Coverage\]\(https://img\.shields\.io/badge/test__coverage-[^)]+\)\]?(?:\([^)]+\))?\n?",
        "",
        readme,
    )
    readme = re.sub(
        r"\[?!\[(?:Doc|doc)[ _]?Coverage\]\(https://img\.shields\.io/badge/doc__coverage-[^)]+\)\]?(?:\([^)]+\))?\n?",
        "",
        readme,
    )

    # Insert new shields next to the license shield
    license_shield = (
        r"(\[!\[License\]\(https://img\.shields\.io/badge/license-[^)]+\)\]\([^)]+\))"
    )

    new_shields = "\\1\n" + doc_shield + "\n" + test_shield

    new_readme = re.sub(license_shield, new_shields, readme, count=1)

    with open("README.md", "w") as f:
        f.write(new_readme)


if __name__ == "__main__":
    update_readme()
