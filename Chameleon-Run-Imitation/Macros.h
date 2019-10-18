#pragma once

/*
PCH: Yes
*/

#define ReleaseCOM(o) { if (o) { (o)->Release(); (o) = nullptr; } }