{
  lib,
  stdenv,
  gnumake,
  pkg-config,
  freetype,
  fontconfig,
  libGL,
  shared_lib ? true,
}:
stdenv.mkDerivation (finalAttrs: {
  pname = "lato";
  version = "0.1.0";

  src = ./.;

  depsBuildBuild = [
    pkg-config
  ];

  nativeBuildInputs = [
    pkg-config
    gnumake
  ];

  buildInputs = [
    freetype
    fontconfig
    libGL
  ];

  buildPhase = ''
    make
  '';

  installPhase =
    if shared_lib
    then ''
      mkdir -p $out/lib
      cp liblato.so $out/lib/
    ''
    else ''
      mkdir -p $out/lib
      cp liblato.a $out/lib/
    '';

  meta = with lib; {
    description = "Text rendering library";
    license = licenses.mit;
    platforms = platforms.linux;
  };
})
