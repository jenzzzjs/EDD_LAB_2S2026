pasos para instalar c++ en windows 

1- colocar el siguiente comando en visual 


```powershell
winget install Microsoft.VisualStudioCode; winget install Git.Git; winget install MSYS2.MSYS2
```

2- luego de la instalacion aparecera lo siguiente

![alt text](img/instalacion.png)

Seleccionaremos la opcion numero 1, MSYS2 UCRT64

y colocaremos el siguiente comando

pacman -Syu

Seguido de esto nos va a aparecer que reiniciemos la terminal o en su defecto se va a cerrar sola

volvemos a entrar y colocamos lo siguiente

```bash
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
```

3- Instalacion en visual 

luego de haber hecho lo anterior lo que deben de realizar en visual studio es lo siguiente

en la carpeta donde van a estar desarrollando sus proyectos coloquen lo siguiente

```c++
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\msys64\ucrt64\bin", [EnvironmentVariableTarget]::User)
```

esto le va a dar permisos de administrador a esa carpeta en especifico

luego solo tocarian las instalaciones correspondientes

```c++
code --install-extension ms-vscode.cpptools
```

![alt text](img/instalacion2.png)

se tendria que ver asi

por ultimo debemos de cerrar obligatoriamente, la terminal y el visual studio code 

volvemos a entrar e ingresamos el siguiente comando

```c++
g++ --version
```

![alt text](img/verificacion.png)

y les tendria que retornar eso



