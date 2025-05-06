#pragma once



namespace bdsp
{
	class RangedSlider;
	class SliderPopupMenu : public PopupMenu
	{
	public:
		SliderPopupMenu(GUI_Universals* universalsToUse);
		void setRanged(RangedSlider* rangedP);

		void resized() override;

		void linkToSlider(BaseSlider* s);

	private:
		BaseSlider* parent;
		RangedSlider* rangedParent = nullptr;
		bool isRanged = false;
	};

} // namespace bdsp