#include "dialog.h"
#include "ui_dialog.h"
#include <vector>
#include <cmath>

using namespace std;

const static double G = 0.1f;
const static double scale = 50;
const static double interval_s = 0.01;


struct vector2
{
    vector2() {}
    vector2(double x, double y) : x(x), y(y) {}
    double x = 0.f;
    double y = 0.f;

    vector2 & operator +=(const vector2 &o)
    {
        x += o.x;
        y += o.y;
        return *this;
    }

    vector2 & operator -=(const vector2 &o)
    {
        x -= o.x;
        y -= o.y;
        return *this;
    }

    vector2 & operator *=(float factor)
    {
        x *= factor;
        y *= factor;
        return *this;
    }

    vector2 operator *(float factor)
    {
        vector2 ret = *this;
        ret.x *= factor;
        ret.y *= factor;
        return ret;
    }

    vector2 operator /(float factor)
    {
        vector2 ret = *this;
        ret.x /= factor;
        ret.y /= factor;
        return ret;
    }

    vector2 operator +(const vector2 &o)
    {
        vector2 ret = *this;
        ret.x += o.x;
        ret.y += o.y;
        return ret;
    }

    vector2 operator -(const vector2 &o)
    {
        vector2 ret = *this;
        ret.x -= o.x;
        ret.y -= o.y;
        return ret;
    }

    vector2 & normalize()
    {
        double r = sqrt(x*x + y*y);
        x /= r;
        y /= r;
        return *this;
    }
};

struct body
{
    body() {}
    vector2 pos;
    vector2 prev_pos;
    vector2 vel;
    double m = 1.f;
};

static struct simulation
{
    simulation()
    {
        sun.m = 10.f;
        euler.m = 1E-10;
        verlet.m = 1E-10;

        double start_speed_val = 1.f;

        sun.pos = vector2(0.f, 0.f);
        euler.pos = vector2(-1.f, 0.f);
        euler.vel = vector2(0.f, start_speed_val);
        verlet.pos = vector2(1.f, 0.f);
        verlet.prev_pos = verlet.pos - vector2(0.f, -start_speed_val) * interval_s;

        bodies.push_back(&sun);
        bodies.push_back(&euler);
        bodies.push_back(&verlet);
    }

    body sun;
    body euler;
    body verlet;

    vector<body *> bodies;
} s_simulation;


Dialog::Dialog(QWidget *parent) :
    QDialog(parent)
  , ui(new Ui::Dialog)
  , m_timer(this)
{
    ui->setupUi(this);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(on_timer()));
    m_timer.setInterval(interval_s*1000.f);
    m_timer.start();
}

Dialog::~Dialog()
{
    delete ui;
}

vector2 calc_a(body *b, const vector<body *> &other)
{
    vector2 a;

    for (auto &o : other)
    {
//        if (o == b)
        if (o != &s_simulation.sun) // temporary skip all except sun
        {
            continue;
        }

        vector2 diff = b->pos - o->pos;
        double r_2 = diff.x*diff.x + diff.y*diff.y;
        a -= diff.normalize()*(o->m / r_2 * G);
    }

    return a;
}

void Dialog::step_simulation(double dt)
{
    body *euler = &s_simulation.euler;
    vector2 f_euler = calc_a(euler, s_simulation.bodies);
    euler->pos += euler->vel * dt;
    euler->vel += f_euler * dt;

    body *verlet = &s_simulation.verlet;
    vector2 f_verlet = calc_a(verlet, s_simulation.bodies);
    vector2 pos = verlet->pos;
    verlet->pos = pos * 2.f - verlet->prev_pos + f_verlet * dt * dt;
    verlet->prev_pos = pos;
}

void Dialog::draw()
{
    QSize screen = this->size();
    QSize half = screen / 2.f;
    QPoint mid(half.width(), half.height());

    auto transform = [&](const vector2 &p) -> QPoint
    {
        vector2 ret = p;
        ret *= scale;
        ret.y = -ret.y;
        ret += vector2((double)mid.x(), (double)mid.y());
        return QPoint(ret.x, ret.y);
    };

    QPoint pos;

    pos = transform(s_simulation.verlet.pos);
    ui->verlet->setGeometry(pos.x(), pos.y(), 70, 20);

    pos = transform(s_simulation.euler.pos);
    ui->euler->setGeometry(pos.x(), pos.y(), 70, 20);

    pos = transform(s_simulation.sun.pos);
    ui->sun->setGeometry(pos.x(), pos.y(), 70, 20);
}

void Dialog::on_timer()
{
    step_simulation(interval_s);
    draw();
}
