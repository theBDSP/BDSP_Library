#pragma once
namespace bdsp
{
	class BPMComponentLNF : public LookAndFeel
	{
	public:
		BPMComponentLNF(GUI_Universals* universalsToUse)
			:LookAndFeel(universalsToUse)
		{

		}
		~BPMComponentLNF()
		{

		}

		void drawLabel(juce::Graphics& g, juce::Label& label) override
		{
			//auto alpha = label.isEnabled() ? 1.0f : universals->disabledAlpha;

			//auto core = dynamic_cast<ComponentCore*>(&label);
			//jassert(core != nullptr);





			//const auto& textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());


			//g.setFont(resizeFontToFit(getFont(core->getFontIndex()), textArea.getWidth(), textArea.getHeight(), "1000.0 BPM"));

			//if (!label.isBeingEdited())
			//{

			//	g.setColour(universals->hoverAdjustmentFunc(label.findColour(juce::Label::textColourId), core->isHovering()).withMultipliedAlpha(alpha));

			//	drawText(g, g.getCurrentFont(),label.getText(), textArea, false,label.getJustificationType());

			///*	juce::GlyphArrangement ga;
			//	juce::Path p;
   //             
   //             ga.addLineOfText(g.getCurrentFont(),label.getText(),0,0);
   //             ga.createPath(p);
   //             
   //             p.applyTransform(juce::AffineTransform().translated(textArea.getCentreX()-p.getBounds().getCentreX(), textArea.getCentreY()-p.getBounds().getCentreY()));
   //             g.fillPath(p);*/


			//}


			//if (label.getCurrentTextEditor() != nullptr)
			//{


			//	label.getCurrentTextEditor()->applyFontToAllText(g.getCurrentFont());


			//	label.getCurrentTextEditor()->setJustification(label.getJustificationType());
			//	label.getCurrentTextEditor()->setIndents(0, 0);
			//	label.getCurrentTextEditor()->setBorder(juce::BorderSize<int>(0));
			//	label.getCurrentTextEditor()->setPopupMenuEnabled(false);

			//	label.getCurrentTextEditor()->setInputRestrictions(-1, "0123456789.");

	
			//}



		}
	};
}// namnepace bdsp
