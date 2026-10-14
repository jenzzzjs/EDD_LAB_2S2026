#include "Graphviz.h"

#include <atomic>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace graphviz {
namespace {

std::string g_error;
std::atomic<int> g_contador{0};

// Arma un nombre unico en la carpeta temporal para no chocar cuando hay
// varias peticiones generando su imagen al mismo tiempo.
std::string tempPath(const std::string& ext) {
    std::filesystem::path dir = std::filesystem::temp_directory_path();
    int n = ++g_contador;
#ifdef _WIN32
    DWORD pid = GetCurrentProcessId();
#else
    int pid = 0;
#endif
    std::ostringstream oss;
    oss << "red_" << pid << "_" << n << ext;
    return (dir / oss.str()).string();
}

// Corre `dot -Tpng entrada -o salida` escondido (sin ventana en Windows)
// y espera maximo 15 segundos. Devuelve true si el `dot` salio con 0.
bool ejecutarDot(const std::string& in, const std::string& out) {
    std::string cmd = "dot -Tpng \"" + in + "\" -o \"" + out + "\" 2>&1";
#ifdef _WIN32
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    std::string cmdLine = "cmd /c " + cmd;
    if (!CreateProcessA(nullptr, cmdLine.data(), nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
        return false;
    WaitForSingleObject(pi.hProcess, 15000);
    DWORD code = 1;
    GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return code == 0;
#else
    return system(cmd.c_str()) == 0;
#endif
}

}  // namespace

// En Windows se pregunta con `where dot`, en Linux con `command -v dot`.
bool disponible() {
#ifdef _WIN32
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    char cmd[] = "cmd /c where dot > NUL 2>&1";
    if (!CreateProcessA(nullptr, cmd, nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
                        nullptr, nullptr, &si, &pi))
        return false;
    WaitForSingleObject(pi.hProcess, 5000);
    DWORD code = 1;
    GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return code == 0;
#else
    return system("command -v dot >/dev/null 2>&1") == 0;
#endif
}

std::string ultimoError() { return g_error; }

// El truco completo: se guarda el DOT en un .dot temporal, se llama al
// `dot` para que genere el .png, se leen los bytes y se borran los dos
// temporales para no dejar basura. Igualito a como se hacia en C++ en las
// semanas pasadas, solo que aqui el PNG viaja al frontend en vez de a disco.
std::string renderDotAPng(const std::string& dot) {
    g_error.clear();
    std::string in = tempPath(".dot");
    std::string out = tempPath(".png");
    {
        std::ofstream fh(in, std::ios::binary);
        if (!fh) {
            g_error = "no se pudo escribir DOT temporal";
            return "";
        }
        fh << dot;
    }
    bool ok = ejecutarDot(in, out);
    std::error_code ec;
    std::filesystem::remove(in, ec);
    if (!ok) {
        g_error = "fallo el binario `dot` de Graphviz";
        std::filesystem::remove(out, ec);
        return "";
    }
    std::ifstream fh(out, std::ios::binary);
    if (!fh) {
        g_error = "dot no genero el PNG";
        return "";
    }
    std::ostringstream ss;
    ss << fh.rdbuf();
    fh.close();
    std::filesystem::remove(out, ec);
    std::string png = ss.str();
    if (png.size() < 8) {
        g_error = "PNG vacio";
        return "";
    }
    return png;
}

}  // namespace graphviz
