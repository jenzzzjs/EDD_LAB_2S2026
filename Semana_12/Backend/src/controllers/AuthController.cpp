#include "AuthController.h"

#include "../models/Store.h"
#include "../utils/Json.h"
#include "../utils/Sha256.h"

namespace AuthController {

// El correo tiene que traer @ y un punto despues, validacion sencilla
// para no dejar pasar cualquier cosa al registro.
static bool correoValido(const std::string& c) {
    size_t at = c.find('@');
    if (at == std::string::npos || at == 0) return false;
    return c.find('.', at) != std::string::npos;
}

// Registro: solo se piden 3 datos (nombre, correo y password), que es lo
// unico que el frontend manda. Se guarda el hash, nunca la clave en claro,
// y de paso la persona entra como vertice nuevo al grafo (todavia sin amigos).
Result registro(const std::string& nombre, const std::string& correo,
                const std::string& password) {
    std::string n = trimStr(nombre);
    std::string c = toLowerStr(trimStr(correo));
    if (n.empty() || c.empty() || password.empty())
        return {400, errJson("nombre, correo y password son obligatorios")};
    if (!correoValido(c)) return {400, errJson("correo invalido")};
    if (password.size() < 4)
        return {400, errJson("la password debe tener al menos 4 caracteres")};

    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);
    if (st.buscarUsuario(c) >= 0)
        return {409, errJson("el correo ya esta registrado")};

    Usuario u;
    u.nombre = n;
    u.correo = c;
    u.passwordHash = sha256::hashHex(password);
    st.usuarios.push_back(u);
    st.grafo.agregarVertice(n, c);

    Result r;
    r.status = 201;
    r.body = "{\"ok\":true,\"usuario\":{\"nombre\":" + json::str(n) + ",\"correo\":" +
             json::str(c) + "}}";
    return r;
}

// Login: se busca el correo y se compara el hash. Si algo no cuadra se
// devuelve 401 sin decir si fallo el correo o la clave (por seguridad).
Result login(const std::string& correo, const std::string& password) {
    std::string c = toLowerStr(trimStr(correo));
    if (c.empty() || password.empty())
        return {400, errJson("correo y password son obligatorios")};

    Store& st = Store::get();
    std::lock_guard<std::mutex> lock(st.mtx);
    int ui = st.buscarUsuario(c);
    if (ui < 0 || !sha256::verify(password, st.usuarios[ui].passwordHash))
        return {401, errJson("credenciales invalidas")};

    Result r;
    r.body = "{\"ok\":true,\"usuario\":{\"nombre\":" + json::str(st.usuarios[ui].nombre) +
             ",\"correo\":" + json::str(st.usuarios[ui].correo) + "}}";
    return r;
}

}  // namespace AuthController
