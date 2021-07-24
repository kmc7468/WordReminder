#include "LayoutEngine.hpp"

#include <cassert>
#include <utility>

Length::Length(std::function<double(const LengthEvaluationContext&)> evaluator) noexcept
	: m_Evaluator(std::move(evaluator)) {
	assert(m_Evaluator != nullptr);
}

void Length::Evaluate(const LengthEvaluationContext& evaluationContext) {
	m_Evaluated = m_Evaluator(evaluationContext);

	assert(m_Evaluated != 0);
}
double Length::GetEvaluated() const noexcept {
	assert(m_Evaluated != 0);

	return m_Evaluated;
}

Length Length::Pixel(double value, ScaleMode scaleMode) {
	return Length([value, scaleMode](const LengthEvaluationContext& evaluationContext) {
		return value * (scaleMode == DisplayBased ? evaluationContext.DisplayScale : evaluationContext.WindowScale);
	});
}
Length Length::Percent(double value) {
	return Length([value](const LengthEvaluationContext& evaluationContext) {
		return evaluationContext.ParentLength * value / 100;
	});
}
Length Length::WrapContent() {
	return Length([](const LengthEvaluationContext&) {
		return -1;
	});
}