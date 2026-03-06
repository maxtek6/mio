#!/usr/bin/env python

# Copyright (c) 2026 Maxtek Consulting

#	Permission is hereby granted, free of charge, to any person obtaining a copy
#	of this software and associated documentation files (the "Software"), to deal
#	in the Software without restriction, including without limitation the rights
#	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#	copies of the Software, and to permit persons to whom the Software is
#	furnished to do so, subject to the following conditions:

#	The above copyright notice and this permission notice shall be included in all
#	copies or substantial portions of the Software.

#	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#	SOFTWARE.

# amalgamate.py - Amalgamate C source and header files.
# Copyright (c) 2012, Erik Edlund <erik.edlund@32767.se>
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
# 
#  * Redistributions of source code must retain the above copyright notice,
#  this list of conditions and the following disclaimer.
# 
#  * Redistributions in binary form must reproduce the above copyright notice,
#  this list of conditions and the following disclaimer in the documentation
#  and/or other materials provided with the distribution.
# 
#  * Neither the name of Erik Edlund, nor the names of its contributors may
#  be used to endorse or promote products derived from this software without
#  specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import argparse
import datetime
import json
import re
from pathlib import Path
from typing import Iterable, Optional

import networkx as nx


class Amalgamate:
    """
    Amalgamate C/C++ source and header files.
    """
    def __init__(
        self,
        source_path: Path,
        target_path: Path,
        *,
        version_path: Optional[Path] = None,
        license_path: Optional[Path] = None,
        ignore_files: Optional[Iterable[str]] = None,
        project_name: Optional[str],
        project_description : Optional[str],
        verbose: bool = False,
    ) -> None:
        # Note : All paths are given relativly to the project root directory 
        # (where the main CMakeLists.txt located on user machine)
        self.verbose = verbose
        self.source_path = Path(source_path)
        self.target_path = Path(target_path)
        self.version_path = Path(version_path) if version_path else None
        self.license_path = Path(license_path) if license_path else None
        # list of includes/source code files to be ignored from amalgamate 
        self.ignore_files = list(ignore_files) if ignore_files else []
        # full path to Amalgamate class script in third_party directory 
        self.script_directory = Path(__file__).resolve().parent
        self.root_directroy = self.script_directory.parent.parent
        # project metadata 
        self.project_name = project_name
        self.project_description = project_description
        # misc
        self.file_pattern = ["*.hpp","*.h","*.hh","*.hxx", "*.ipp"]
        self.include_pattern = re.compile(r'^\s*#\s*include\s*["<]([^">]+)[">]', re.ASCII)

        if not self.source_path.exists() or not self.source_path.is_dir():
            raise ValueError(f"source_path is not a directory: {self.source_path}")

        target_parent = self.target_path.parent
        if target_parent and not target_parent.exists():
            target_parent.mkdir(parents=True, exist_ok=True)

    def generate_header(self) -> None:
        
        open(self.target_path, "w").close()  
        self._write_notice()
        self._write_timestamp()
        self._write_license_notice()
        self._write_header_guard_begin()
        self._write_version_macros()

        root_dir = (self.root_directroy / self.source_path).resolve()
        G = self._build_include_graph(root_dir)
        # for node, neighbors in G.adj.items():
        #     print(node, "->", list(neighbors))
        try:
            order = list(nx.topological_sort(G))
        except nx.NetworkXUnfeasible:
            cycle = nx.find_cycle(G)
            raise RuntimeError(f"[amalgamate] : cyclic include detected: {cycle}")
        
        print(f'[amalgamate] Starting  ...')
        with open(self.target_path, "a", encoding="utf-8") as out:
            for path in order:
                if any(ig in str(path) for ig in self.ignore_files):
                    if self.verbose:
                        print(f"[amalgamate] skipping ignored file: {path}")
                    continue

                if self.verbose:
                    print(f"[amalgamate] adding file: {path}")

                try:
                    text = path.read_text(encoding="utf-8")
                except Exception as e:
                    print(f"[amalgamate] ERROR reading {path}: {e}")
                    continue
                text = self._strip_header_guard_begin(text)
                text = self._strip_header_guard_end(text)
                
                out.write(f"{text}")
                out.write("\n\n")

        if self.verbose:
            print("[amalgamate] Generation finished.")
        
        self._write_header_guard_end()

    def _write_notice(self) -> None:
        with open(self.target_path, "a", encoding="utf-8") as f:
            f.write("\n//===============================================================\n")
            f.write("// AUTOGENERATED BY AMALGAMATE SCRIPT — DO NOT EDIT BY HAND!\n")
            f.write("//===============================================================\n")

    def _write_timestamp(self) -> None:
        timestamp = datetime.datetime.now().isoformat()
        with open(self.target_path, "a", encoding="utf-8") as f:
            f.write(f"// Generated on {timestamp}\n\n")

    def _write_version_macros(self) -> None:
        # write library version marcos, from VERSION file 
        if not self.version_path or not self.version_path.exists():
            raise ValueError(f"version file does not exist: {self.version_path}")

        version_str = self.version_path.read_text().strip()
        major, minor, patch = version_str.split(".")

        macro = self.project_name.upper() # type: ignore
        with open(self.target_path, "a", encoding="utf-8") as f:
            f.write(f"#define {macro}_VERSION_MAJOR {major}\n")
            f.write(f"#define {macro}_VERSION_MINOR {minor}\n")
            f.write(f"#define {macro}_VERSION_PATCH {patch}\n\n")

    def _write_license_notice(self) -> None:
        # write library license notice from LICENSE file
        if not self.license_path or not self.license_path.exists():
            raise ValueError(f"license file does not exist: {self.license_path}")
        lic = self.license_path.read_text()
        with open(self.target_path, "a", encoding="utf-8") as f:
            f.write("/*\n")
            f.write(lic)
            f.write("\n*/\n\n")

    def _write_header_guard_begin(self)->None:
        # this function constructs the single header preprocessor, from 
        # the project name 
        macro = self.project_name.upper() # type: ignore
        with open(self.target_path, "a", encoding="utf-8") as f:
            f.write(f"\n#ifndef {macro}_HPP\n")
            f.write(f"#define {macro}_HPP\n\n")
        return 
    
    def _write_header_guard_end(self)->None: 
        # same as _write_header_guard_begin for closing 
        macro = self.project_name.upper() # type: ignore
        with open(self.target_path, "a", encoding="utf-8") as f:
            f.write(f"#endif // {macro}_HPP ")
        return 

    def _build_include_graph(self, root_dir: Path):
        # build a directed graph of file includes, where nodes are file 
        # paths and edges represent #include relationships.
        G = nx.DiGraph()
        root_files = [
            p
            for pattern in self.file_pattern
            for p in root_dir.glob(pattern)
            if p.is_file()
        ]
        for src in root_files:
            G.add_node(src)
            includes = self._extract_includes(src)
            for inc in includes:
                inc_path = (root_dir / inc).resolve()
                if inc_path.exists():
                    G.add_edge(src, inc_path)
                    # get the file sub-includes and add them to the graph
                    child_includes = self._extract_includes(inc_path)
                    if self.verbose:
                        print(f'    Processing   :  {inc_path} ...')
                        print(f'       ->        : {child_includes}')
                    for child_inc in child_includes :
                        child_inc_path = (root_dir / child_inc).resolve()
                        if child_inc_path.exists():
                            G.add_edge(inc_path, child_inc_path)
                        else:
                            print(f'    Unable to find include file {child_inc_path} - Skip')

                else:
                    if self.verbose:
                        print(f"Unable resolve include file path {inc_path}")
        return G

    def _extract_includes(self, file_path: Path) -> list[str]:
        # extract #include directives from a file and 
        # return a list of included file paths.
        includes = []
        try:
            text = file_path.read_text(encoding="utf-8", errors="ignore")
        except Exception:
            return includes
        for line in text.splitlines():
            m = self.include_pattern.match(line)
            if m:
                includes.append(m.group(1))  
        return includes


    def _strip_header_guard_begin(self, text: str) -> str:
        # remove the first header guard 
        lines = text.splitlines()
        n = len(lines)
        i = 0
        while i < n and not lines[i].lstrip().startswith("#ifndef"):
            i += 1
        if i == n:
            return text
        i += 1
        while i < n and not lines[i].lstrip().startswith("#define"):
            i += 1
        if i < n and lines[i].lstrip().startswith("#define"):
            i += 1  
        while i < n and lines[i].strip() == "":
            i += 1

        return "\n".join(lines[i:])

    def _strip_header_guard_end(self, text: str) -> str:
        # remove the trailing #endif header guard
        lines = text.splitlines()
        i = len(lines) - 1

        while i >= 0 and lines[i].strip() == "":
            i -= 1
        if i >= 0 and lines[i].lstrip().startswith("#endif"):
            i -= 1
        while i >= 0 and lines[i].strip() == "":
            i -= 1

        return "\n".join(lines[:i + 1]) + ("\n" if i >= 0 else "")

def main():
    parser = argparse.ArgumentParser(
        description="Amalgamate C/C++ source and header files"
    )

    parser.add_argument("-v", "--verbose", action="store_true", 
                        help="enable verbose logging")
    parser.add_argument("-c", "--config", required=False, help=
                        "path to configuration file (eg config.json)")
    parser.add_argument("-s", "--source", required=True, 
                        help="path to project source directory")
    parser.add_argument("-t", "--target", required=True, 
                        help="path to target autogenerated header file")

    args = parser.parse_args()

    with open(args.config) as cfg:
        config = json.load(cfg)

    am = Amalgamate(
        source_path=Path(args.source),
        target_path=Path(args.target),
        version_path=Path(config.get("version", "")) if config.get("version") else None,
        license_path=Path(config.get("license", "")) if config.get("license") else None,
        ignore_files=config.get("ignore", []),
        project_name= str(config.get("name")),
        project_description= config.get("description"),
        verbose=args.verbose,
    )

    am.generate_header()


if __name__ == "__main__":
    main()

