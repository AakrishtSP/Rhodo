
namespace Rhodo
{
	__declspec(dllimport) void Print();
};



int main() {

	Rhodo::Print();

	while (true) {}
	return 0;
}