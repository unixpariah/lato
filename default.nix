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

  postFixup = ''
    mkdir $out/lib/pkgconfig
    echo "prefix=$out
    exec_prefix=$out
    libdir=$out/lib
    includedir=$out/include

    Name: lato
    Description: Text rendering library
    Version: 1.1.3
    Libs: -L$out/lib -llato
    Cflags: -I$out/include" > $out/lib/pkgconfig/lato.pc
  '';

  installPhase =
    if shared_lib
    then ''
      mkdir -p $out/lib $out/include/lato
      cp lib/liblato.so $out/lib
      cp include/* $out/include/lato
    ''
    else ''
      mkdir -p $out/lib $out/include/lato
      cp lib/liblato.a $out/lib
      cp include/* $out/include/lato
    '';

  meta = with lib; {
    description = "Text rendering library";
    license = licenses.mit;
    platforms = platforms.linux;
  };
})
