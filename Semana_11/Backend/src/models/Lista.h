#pragma once
// Lista enlazada simple hecha a mano: nodos con new/delete, cabeza + cola
// (push_back en O(1)) y contador de tamaño. No usa std::vector ni ningun
// otro contenedor STL: es la estructura del laboratorio.

#include <cstddef>
#include <string>

// Par clave=valor hecho a mano (para query strings, cabeceras y el
// indice de IDs del DOT). Sin std::map ni std::pair.
struct Par {
    std::string clave;
    std::string valor;
};

template <typename T>
class Lista {
private:
    struct Nodo {
        T dato;
        Nodo* sig;
        explicit Nodo(const T& d) : dato(d), sig(nullptr) {}
    };
    Nodo* cabeza_;
    Nodo* cola_;
    size_t n_;

    void copiarDe(const Lista& o) {
        for (Nodo* p = o.cabeza_; p != nullptr; p = p->sig) push_back(p->dato);
    }

public:
    Lista() : cabeza_(nullptr), cola_(nullptr), n_(0) {}

    Lista(const Lista& o) : cabeza_(nullptr), cola_(nullptr), n_(0) { copiarDe(o); }

    Lista(Lista&& o) noexcept : cabeza_(o.cabeza_), cola_(o.cola_), n_(o.n_) {
        o.cabeza_ = nullptr;
        o.cola_ = nullptr;
        o.n_ = 0;
    }

    Lista& operator=(const Lista& o) {
        if (this != &o) {
            limpiar();
            copiarDe(o);
        }
        return *this;
    }

    Lista& operator=(Lista&& o) noexcept {
        if (this != &o) {
            limpiar();
            cabeza_ = o.cabeza_;
            cola_ = o.cola_;
            n_ = o.n_;
            o.cabeza_ = nullptr;
            o.cola_ = nullptr;
            o.n_ = 0;
        }
        return *this;
    }

    ~Lista() { limpiar(); }

    // Mete al final. Con la cola guardada no hay que recorrer nada.
    void push_back(const T& d) {
        Nodo* n = new Nodo(d);
        if (cabeza_ == nullptr) {
            cabeza_ = n;
            cola_ = n;
        } else {
            cola_->sig = n;
            cola_ = n;
        }
        n_++;
    }

    size_t tam() const { return n_; }
    bool vacia() const { return n_ == 0; }

    void limpiar() {
        Nodo* p = cabeza_;
        while (p != nullptr) {
            Nodo* s = p->sig;
            delete p;
            p = s;
        }
        cabeza_ = nullptr;
        cola_ = nullptr;
        n_ = 0;
    }

    // Acceso por posicion recorriendo desde la cabeza (O(n), normal en
    // una enlazada simple; el laboratorio no mueve miles de datos).
    T& operator[](size_t i) {
        Nodo* p = cabeza_;
        for (size_t k = 0; k < i && p != nullptr; k++) p = p->sig;
        return p->dato;
    }
    const T& operator[](size_t i) const {
        Nodo* p = cabeza_;
        for (size_t k = 0; k < i && p != nullptr; k++) p = p->sig;
        return p->dato;
    }
};
