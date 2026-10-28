{pkgs, ...}: {
  packages = with pkgs;
  with pkgs.qt6; [
    qtcreator
    qtbase
    cmake
    qttools
    wrapQtAppsHook
    qtsvg
    qtimageformats
  ];
}
