#include <QApplication> // Clase principal de Qt para la aplicacion
#include <QWidget> // Ventana y widgets base
#include <QVBoxLayout> // Layout vertical
#include <QHBoxLayout> // Layout horizontal
#include <QGridLayout> // Layout en cuadricula para el formulario
#include <QLabel> // Etiquetas de texto
#include <QLineEdit> // Campos de texto para entrada de datos
#include <QPushButton> // Botones de accion
#include <QTableWidget> // Tabla para mostrar los estudiantes
#include <QHeaderView> // Cabeceras de la tabla
#include <QMessageBox> // Ventanas de mensaje como alertas
#include "sistema.h" // Cabecera del sistema de estudiantes

// Actualiza la tabla de la interfaz con los datos actuales de la lista enlazada
void actualizarTabla(QTableWidget *table, ListaEnlazada &lista) {
    table->setRowCount(0); // Limpiamos la tabla
    Nodo* actual = lista.getCabeza();
    // Recorremos la lista y agregamos una fila por cada estudiante
    while (actual != nullptr) {
        int fila = table->rowCount();
        table->insertRow(fila);
        table->setItem(fila, 0, new QTableWidgetItem(QString::fromStdString(actual->nombre)));
        table->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(actual->carnet)));
        table->setItem(fila, 2, new QTableWidgetItem(QString::fromStdString(actual->carrera)));
        actual = actual->siguiente;
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Sistema sistema; // Instancia del sistema que maneja los estudiantes

    // Configuracion de la ventana principal
    QWidget window;
    window.setWindowTitle("Gestion de Estudiantes");
    window.resize(600, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(&window);

    // Formulario para ingresar datos del estudiante
    QGridLayout *formLayout = new QGridLayout();
    QLabel *lblNombre = new QLabel("Nombre:");
    QLineEdit *txtNombre = new QLineEdit();
    QLabel *lblCarnet = new QLabel("Carnet:");
    QLineEdit *txtCarnet = new QLineEdit();
    QLabel *lblCarrera = new QLabel("Carrera:");
    QLineEdit *txtCarrera = new QLineEdit();

    formLayout->addWidget(lblNombre, 0, 0);
    formLayout->addWidget(txtNombre, 0, 1);
    formLayout->addWidget(lblCarnet, 1, 0);
    formLayout->addWidget(txtCarnet, 1, 1);
    formLayout->addWidget(lblCarrera, 2, 0);
    formLayout->addWidget(txtCarrera, 2, 1);

    mainLayout->addLayout(formLayout);

    // Botones de accion: Insertar, Buscar, Eliminar y Reporte
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *btnInsertar = new QPushButton("Insertar");
    QPushButton *btnBuscar = new QPushButton("Buscar");
    QPushButton *btnEliminar = new QPushButton("Eliminar");
    QPushButton *btnReporte = new QPushButton("Reporte");

    buttonLayout->addWidget(btnInsertar);
    buttonLayout->addWidget(btnBuscar);
    buttonLayout->addWidget(btnEliminar);
    buttonLayout->addWidget(btnReporte);

    mainLayout->addLayout(buttonLayout);

    // Tabla que muestra todos los estudiantes registrados
    QTableWidget *table = new QTableWidget();
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Nombre", "Carnet", "Carrera"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mainLayout->addWidget(table);

    // Boton Insertar: agrega un nuevo estudiante a la lista
    QObject::connect(btnInsertar, &QPushButton::clicked, [&]() {
        string nombre = txtNombre->text().toStdString();
        string carnet = txtCarnet->text().toStdString();
        string carrera = txtCarrera->text().toStdString();

        // Validamos que todos los campos esten llenos
        if (nombre.empty() || carnet.empty() || carrera.empty()) {
            QMessageBox::warning(&window, "Error", "Todos los campos son obligatorios.");
            return;
        }

        sistema.insertar(nombre, carnet, carrera);
        actualizarTabla(table, sistema.getLista());

        // Limpiamos los campos despues de insertar
        txtNombre->clear();
        txtCarnet->clear();
        txtCarrera->clear();
    });

    // Boton Buscar: busca un estudiante por carnet y muestra sus datos
    QObject::connect(btnBuscar, &QPushButton::clicked, [&]() {
        string carnet = txtCarnet->text().toStdString();

        if (carnet.empty()) {
            QMessageBox::warning(&window, "Error", "Ingrese un carnet para buscar.");
            return;
        }

        Nodo* resultado = sistema.buscar(carnet);
        if (resultado != nullptr) {
            // Mostramos los datos encontrados en los campos del formulario
            txtNombre->setText(QString::fromStdString(resultado->nombre));
            txtCarrera->setText(QString::fromStdString(resultado->carrera));

            // Seleccionamos la fila correspondiente en la tabla
            for (int i = 0; i < table->rowCount(); i++) {
                if (table->item(i, 1)->text().toStdString() == carnet) {
                    table->selectRow(i);
                    break;
                }
            }
        } else {
            QMessageBox::information(&window, "Resultado", "No se encontro el estudiante.");
        }
    });

    // Boton Eliminar: elimina un estudiante por carnet
    QObject::connect(btnEliminar, &QPushButton::clicked, [&]() {
        string carnet = txtCarnet->text().toStdString();

        if (carnet.empty()) {
            QMessageBox::warning(&window, "Error", "Ingrese un carnet para eliminar.");
            return;
        }

        if (sistema.eliminar(carnet)) {
            actualizarTabla(table, sistema.getLista());
            // Limpiamos los campos despues de eliminar
            txtNombre->clear();
            txtCarnet->clear();
            txtCarrera->clear();
        } else {
            QMessageBox::information(&window, "Resultado", "No se encontro el estudiante.");
        }
    });

    // Boton Reporte: genera un grafico PNG de la lista enlazada
    QObject::connect(btnReporte, &QPushButton::clicked, [&]() {
        sistema.getLista().graficar();
        QMessageBox::information(&window, "Reporte", "Reporte generado: lista.png");
    });

    window.show();
    return app.exec();
}