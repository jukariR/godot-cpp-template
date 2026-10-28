Secure - cifra tus scripts .gd al exportar a Windows.

Instalar:
1. Copia la carpeta "addons/secure" dentro de tu proyecto (en su carpeta "addons/").
2. Project > Project Settings > pestaña Plugins > activa "Secure".
3. Al exportar a Windows, pon GDScript "Script Export Mode" en "Text".

Nota: incluye solo el binario de Windows (debug). Para release, compila el dll
con target=template_release y ponlo en bin/windows/.
