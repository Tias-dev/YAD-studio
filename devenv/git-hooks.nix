{
  git-hooks.hooks = {
    alejandra = {
      enable = true;
      files = "\\.(nix)$";
    };
    clang-format = {
      enable = true;
      files = "\\.(cpp|h)$";
    };
    prettier = {
      enable = true;
      files = "\\.(js|html|htm|xml)$";
    };
  };
}
