#pragma once

/*
PCH: Yes
*/

#define DestroyCOM(o) { if (o) { (o)->Release(); (o) = nullptr; } }