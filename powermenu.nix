{ pkgs ? import <nixpkgs> {}, stdenv ? pkgs.stdenv }:

stdenv.mkDerivation rec {
  pname = "powermenu";
  version = "1.0";
  
  src = fetchGit {
    url = "https://github.com/ozpv/powermenu.git";
    # run git rev-parse HEAD in this dir and paste here 
    rev = "";
  };

  makeFlags = [ "PREFIX=$(out)" "CC=${stdenv.cc.targetPrefix}cc" ];
}
