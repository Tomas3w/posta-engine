#include <posta/UI/Rect.h>

std::ostream& std::operator<<(std::ostream& out, const posta::ui::Rect& rect)
{
	return out << "Rect{" << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << "}";
}

