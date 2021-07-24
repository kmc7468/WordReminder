#pragma once

class Length final {
public:
	enum ScaleMode {
		DisplayBased,
		WindowBased,
	};

private:
	enum class Unit {
		Pixel,
		Percent,
		WrapContent,
	};

private:
	double m_Value;
	Unit m_Unit;
	ScaleMode m_ScaleMode;

public:
	Length(const Length& length) = default;
	~Length() = default;

private:
	Length(double value, Unit unit, ScaleMode scaleMode) noexcept;

public:
	Length& operator=(const Length& length) = default;

public:
	double Evaluate(double displayScale, double windowScale, double parentLength) const noexcept;

public:
	static Length Pixel(double value, ScaleMode scaleMode = DisplayBased);
	static Length Percent(double value);
	static Length WrapContent();
};