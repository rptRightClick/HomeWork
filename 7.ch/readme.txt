diff -u clib-package.c clib-package-patch.c > memory_fix.patch
команда для создания патча

patch -p1 < memory_fix.patch
для применения патча, если авто не сработает, указать имя файла вручную 


