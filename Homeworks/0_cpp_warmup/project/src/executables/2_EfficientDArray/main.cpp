#include "EDArray.h"

int main(int argc, char **argv) {
	EDArray a;
	a.InsertAt(0, 2.1);
	a.Print();

	a.PushBack(3.0);
	a.PushBack(3.1);
	a.PushBack(3.2);
	a.Print();

	a.DeleteAt(0);
	a.Print();
	a.InsertAt(0, 4.1);
	a.Print();

	EDArray acopy = a;
	acopy.Print();

	EDArray acopy2(a);
	acopy2.Print();

	EDArray acopy3, acopy4;
	acopy4 = acopy3 = a;
	acopy3.Print();
	acopy4.Print();

	EDArray b;
	b.PushBack(21);
	b.Print();
	b.DeleteAt(0);
	b.Print();
	b.PushBack(22);
	b.SetSize(5);
	b.Print();

	EDArray c;
	c.PushBack('a');
	c.PushBack('b');
	c.PushBack('c');
	c.InsertAt(0, 'd');
	c.Print();

	return 0;
}
