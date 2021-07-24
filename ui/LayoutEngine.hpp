#pragma once

#include "../pal/Component.hpp"

#include <functional>
#include <optional>
#include <string>
#include <vector>

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

class LayoutEngine;

class Division final {
	friend class LayoutEngine;

public:
	enum Alignment {
		Left,
		Top = Left,
		Center,
		Right,
		Bottom = Right,
	};

private:
	std::wstring m_Name;
	Component* m_Component = nullptr;
	std::vector<Division> m_Children;

	std::optional<Length> m_LeftMargin, m_RightMargin, m_TopMargin, m_BottomMargin;
	std::optional<Length> m_LeftPadding, m_RightPadding, m_TopPadding, m_BottomPadding;
	Length m_Width, m_Height;

	Alignment m_HorizontalAlignment = Left, m_VerticalAlignment = Top;
	double m_EvaluatedX = 0, m_EvaluatedY = 0;

public:
	Division(std::wstring name, Length width, Length height) noexcept;
	Division(Component* component, Length width, Length height) noexcept;
	Division(std::wstring name, Component* component, Length width, Length height) noexcept;
	Division(const Division&) = delete;
	Division(Division&& division) noexcept = default;
	~Division() = default;

public:
	Division& operator=(const Division&) = delete;
	Division& operator=(Division&& division) noexcept = default;

public:
	void AddChild(Division&& child);

	Division& SetMargin(const Length& margin);
	Division& SetHorizontalMargin(const Length& horizontalMargin);
	Division& SetVerticalMargin(const Length& verticalMargin);
	Division& SetLeftMargin(Length leftMargin) noexcept;
	Division& SetRightMargin(Length rightMargin) noexcept;
	Division& SetTopMargin(Length topMargin) noexcept;
	Division& SetBottomMargin(Length bottomMargin) noexcept;
	Division& SetPadding(const Length& padding);
	Division& SetHorizontalPadding(const Length& horizontalPadding);
	Division& SetVerticalPadding(const Length& verticalPadding);
	Division& SetLeftPadding(Length leftPadding) noexcept;
	Division& SetRightPadding(Length rightPadding) noexcept;
	Division& SetTopPadding(Length topPadding) noexcept;
	Division& SetBottomPadding(Length bottomPadding) noexcept;

	Division& SetHorizontalAlignment(Alignment horizontalAlignment) noexcept;
	Division& SetVerticalAlignment(Alignment verticalAlignment) noexcept;
};