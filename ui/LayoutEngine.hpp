#pragma once

#include <functional>

struct LengthEvaluationContext final {
	double DisplayScale;
	double WindowScale;
	double ParentLength;
	double UsableLength;
};

class Length final {
public:
	enum ScaleMode {
		DisplayBased,
		WindowBased,
	};

private:
	std::function<double(const LengthEvaluationContext&)> m_Evaluator;
	double m_Evaluated = 0;

public:
	explicit Length(std::function<double(const LengthEvaluationContext&)> evaluator) noexcept;
	Length(const Length& length) = default;
	Length(Length&& length) noexcept = default;
	~Length() = default;

public:
	Length& operator=(const Length& length) = default;
	Length& operator=(Length&& length) noexcept = default;

public:
	void Evaluate(const LengthEvaluationContext& evaluationContext);
	double GetEvaluated() const noexcept;

public:
	static Length Pixel(double value, ScaleMode scaleMode = DisplayBased);
	static Length Percent(double value);
	static Length WrapContent();
};