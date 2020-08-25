# MacOS install notes

I have created a branch to house any forthcoming modifications to get the *PMFE* software up, running and functional 
on the MacOS platform. This file documents the procedure:
```bash
git clone https://github.com/gtDMMB/pmfe.git
git checkout maxie-mac-mods-fall2020
#brew install llvm || brew upgrade llvm
#brew install boost || brew reinstall boost
#brew install cgal
brew reinstall boost gmp mpfr cgal
make
```
Now we run the *PMFE* software using the following commands (for example):
```bash

```

