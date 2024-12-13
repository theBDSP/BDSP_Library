#pragma once



namespace bdsp
{
	class RangedSlider;
	class SliderPopupMenu : public PopupMenu
	{
	public:
		SliderPopupMenu(BaseSlider* p, GUI_Universals* universalsToUse);
		void setRanged(RangedSlider* rangedP);

		void resized() override;

	private:
		BaseSlider* parent;
		RangedSlider* rangedParent = nullptr;
		bool isRanged=false;
	};

} // namespace bdsp