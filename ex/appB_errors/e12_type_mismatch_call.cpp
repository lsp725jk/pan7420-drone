#include <string>
int f(const std::string& s) { return (int)s.size(); }
int main() { return f(42); }