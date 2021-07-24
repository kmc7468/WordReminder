#include "LayoutEngine.hpp"

#include <cassert>

Length::Length(double value, Unit unit, ScaleMode scaleMode) noexcept
	: m_Value(value), m_Unit(unit), m_ScaleMode(scaleMode) {}

double Length::Evaluate(double displayScale, double windowScale, double parentLength) const noexcept {
	assert(displayScale > 0);
	assert(windowScale > 0);

	switch (m_Unit) {
	case Unit::Pixel:
		return m_Value * (m_ScaleMode == DisplayBased ? displayScale : windowScale);

	case Unit::Percent:
		assert(parentLength != -1);
		return parentLength * m_Value / 100;

	case Unit::WrapContent:
		return -1;
	}
}

Length Length::Pixel(double value, ScaleMode scaleMode) {
	assert(value > 0);

	return { value, Unit::Pixel, scaleMode };
}
Length Length::Percent(double value) {
	assert(0 < value && value <= 100);

	return { value, Unit::Percent, DisplayBased };
}
Length Length::WrapContent() {
	return { 0.0, Unit::WrapContent, DisplayBased };
}