#!/usr/bin/env bash
set -euo pipefail

workspace_dir="${WORKSPACE_DIR:-/workspace}"
output_dir="${OUTPUT_DIR:-build-win}"
build_type="${BUILD_TYPE:-Release}"
renderer="${D6R_RENDERER:-gl4}"
with_lua="${D6R_WITH_LUA:-ON}"
build_testing="${BUILD_TESTING:-ON}"
run_tests="${RUN_TESTS:-OFF}"
toolchain_file="${TOOLCHAIN_FILE:-/opt/toolchains/mingw-w64-x86_64.cmake}"

tmp_build_dir="$(mktemp -d /tmp/duel6r-build-win-XXXXXX)"
cleanup() {
  rm -rf "${tmp_build_dir}"
}
trap cleanup EXIT

cmake -S "${workspace_dir}" -B "${tmp_build_dir}" \
  -DCMAKE_TOOLCHAIN_FILE="${toolchain_file}" \
  -DCMAKE_BUILD_TYPE="${build_type}" \
  -DBUILD_TESTING="${build_testing}" \
  -DD6R_RENDERER="${renderer}" \
  -DD6R_WITH_LUA="${with_lua}"

icon_file="${workspace_dir}/source/icon1.ico"
if [[ -f "${icon_file}" ]]; then
  cp "${icon_file}" "${tmp_build_dir}/icon1.ico"
fi

cmake --build "${tmp_build_dir}" -j"$(nproc)"

if [[ "${run_tests}" == "ON" ]]; then
  echo "RUN_TESTS=ON is not supported for Windows cross builds in this container." >&2
  echo "Use RUN_TESTS=OFF or execute tests on a native Windows runner." >&2
  exit 1
fi

rm -rf "${workspace_dir}/${output_dir}"
mkdir -p "${workspace_dir}/${output_dir}"

source_binary="${tmp_build_dir}/duel6r.exe"
if [[ ! -f "${source_binary}" && -f "${tmp_build_dir}/duel6rd.exe" ]]; then
  source_binary="${tmp_build_dir}/duel6rd.exe"
fi

if [[ ! -f "${source_binary}" ]]; then
  echo "Unable to find built Windows application in ${tmp_build_dir}" >&2
  exit 1
fi

cp "${source_binary}" "${workspace_dir}/${output_dir}/duel6r.exe"
cp -R "${workspace_dir}/resources/." "${workspace_dir}/${output_dir}/"

python3 - "${workspace_dir}/${output_dir}/duel6r.exe" "${workspace_dir}/${output_dir}" <<'PY'
import pathlib
import shutil
import subprocess
import sys

exe_path = pathlib.Path(sys.argv[1])
dist_dir = pathlib.Path(sys.argv[2])

search_roots = [
    pathlib.Path("/opt/vcpkg/installed/x64-mingw-dynamic/bin"),
    pathlib.Path("/usr/x86_64-w64-mingw32/bin"),
]
search_roots.extend(pathlib.Path("/usr/lib/gcc/x86_64-w64-mingw32").glob("*/"))

seen = set()
queue = [exe_path]


def imported_dlls(path: pathlib.Path) -> list[str]:
    result = subprocess.run(
        ["x86_64-w64-mingw32-objdump", "-p", str(path)],
        capture_output=True,
        text=True,
        check=True,
    )
    dlls: list[str] = []
    for line in result.stdout.splitlines():
        marker = "DLL Name: "
        if marker in line:
            dll_name = line.split(marker, 1)[1].strip()
            if dll_name:
                dlls.append(dll_name)
    return dlls


def resolve_dll(name: str) -> pathlib.Path | None:
    lower_name = name.lower()
    for root in search_roots:
        if not root.exists():
            continue
        candidate = root / name
        if candidate.exists():
            return candidate
        for path in root.glob("*.dll"):
            if path.name.lower() == lower_name:
                return path
    return None


while queue:
    current = queue.pop(0)
    for dll_name in imported_dlls(current):
        normalized = dll_name.lower()
        if normalized in seen:
            continue
        seen.add(normalized)

        resolved = resolve_dll(dll_name)
        if resolved is None:
            continue

        target = dist_dir / resolved.name
        if not target.exists():
            shutil.copy2(resolved, target)
            queue.append(target)
PY

echo "Windows runtime bundle written to ${workspace_dir}/${output_dir}"
