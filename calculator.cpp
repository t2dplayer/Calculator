/****************************************************************************
 Copyright (c) 2020 Sérgio Vieira - sergiosvieira@gmail.com
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#include "calculator.h"
#include "ui_calculator.h"
#include <cmath>

bool IsEqual(double dX, double dY)
{
    const double dEpsilon = 0.000001;
    return fabs(dX - dY) <= dEpsilon * fabs(dX);
}

Calculator::Calculator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Calculator)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog
                   | Qt::MSWindowsFixedSizeDialogHint);
    for (int i = 0; i < 10; ++i) {
        QString buttonName = "b" + QString::number(i);
        QPushButton* button = Calculator::findChild<QPushButton*>(buttonName);
        if (button != nullptr) {
            connect(button,
                    SIGNAL(released()),
                    this,
                    SLOT(numberPressed()));
        }
    }
    structMap = {
        {ui->bsum, [](double a, double b){return a + b;}},
        {ui->bsub, [](double a, double b){return a - b;}},
        {ui->bmul, [](double a, double b){return a * b;}},
        {ui->bdiv, [](double a, double b){
            if (IsEqual(b, 0.0)) return 0.0; return a / b;}
        }
    };
    for (auto [button, value]: structMap) {
        connect(button,
                SIGNAL(released()),
                this,
                SLOT(operationPressed()));
    }
    connect(ui->bclear,
            SIGNAL(released()),
            this,
            SLOT(clearPressed()));
    connect(ui->bequal,
            SIGNAL(released()),
            this,
            SLOT(equalPressed()));

}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::numberPressed()
{
    QPushButton* button = (QPushButton*)sender();
    QString text = button->text();
    if (ui->display->text().length() < 17)
    {
        double value = text.toDouble();
        if (state == State::Ready &&
                !IsEqual(value, 0.0)) {
            state = State::X;
        } else if (state == State::X || state == State::Y) {
            text = ui->display->text() + text;
        } else if (state == State::Op) {
            state = State::Y;
        }
    }
    ui->display->setText(text);
}

void Calculator::operationPressed() {
    QPushButton* button = (QPushButton*)sender();
    currentOp = structMap[button];
    if (state == State::X) {
        state = State::Op;
        x = ui->display->text().toDouble();
    } else if (state == State::Y) {
        state = State::Op;
        y = ui->display->text().toDouble();
    } else if (state == State::Ready) {
        if (currentOp != nullptr) state = State::Op;
    }
    ui->opr->setText(button->text());
}

void Calculator::equalPressed() {
    if (state == State::Y) {
        y = ui->display->text().toDouble();
        x = currentOp(x, y);
        state = State::Ready;
    } else if (state == State::Ready) {
        if (currentOp != nullptr) {
            x = currentOp(x, y);
            state = State::Ready;
        }
    }
    ui->display->setText(QString::number(x));
}

void Calculator::clearPressed() {
    state = State::Ready;
    currentOp = nullptr;
    x = y = 0.0;
    ui->opr->setText("");
    ui->display->setText("0");
}

