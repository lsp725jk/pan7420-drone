struct Base { virtual void f(int) {} };
struct D : Base { void f(double) {} };
int main() { D d; d.f(1); return 0; }