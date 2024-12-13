#include "../melatonin/implementations/gin.h"
#include "../melatonin/shadows.h"
#include "helpers/pixel_helpers.h"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

// All of these tests operate @1x
TEST_CASE ("Melatonin Blur Inner Shadow")
{
    // Test Image (differs from drop shadow, has more inner "meat")
    // 0 is white, 1 is black, the shadow will be *white* in the center

    // 0  0  0  0  0  0  0  0  0
    // 0  0  0  0  0  0  0  0  0
    // 0  0  1  1  1  1  1  0  0
    // 0  0  1  1  1  1  1  0  0
    // 0  0  1  1  1  1  1  0  0
    // 0  0  1  1  1  1  1  0  0
    // 0  0  1  1  1  1  1  0  0
    // 0  0  0  0  0  0  0  0  0
    // 0  0  0  0  0  0  0  0  0

    // create a 5px by 5px square
    auto bounds = juce::Rectangle<float> (5, 5);
    juce::Path p;

    // stick the 5x5 box centered inside a 9x9
    p.addRectangle (bounds.translated (2, 2));

    // needed for JUCE not to pee its pants (aka leak) when working with graphics
    juce::ScopedJuceInitialiser_GUI gui;

    juce::Image result (juce::Image::ARGB, 9, 9, true);

    SECTION ("no shadow (sanity check)")
    {
        {
            juce::Graphics g (result);
            g.fillAll (juce::Colours::white);

            g.setColour (juce::Colours::black);
            g.fillPath (p);
        }
        // nothing should be outside the top left corner
        REQUIRE (getPixel (result, 1, 1) == "FFFFFFFF"); // ARGB

        // starting at pixel 2,2 we have black
        REQUIRE (getPixel (result, 2, 2) == "FF000000");
    }

    SECTION ("default constructor")
    {
        melatonin::InnerShadow shadow;
        {
            juce::Graphics g (result);

            g.fillAll (juce::Colours::white);
            shadow.render (g, p);
        }
        CHECK (isImageFilled (result, juce::Colours::white) == true);
        {
            juce::Graphics g (result);
            shadow.setRadius (5);
            shadow.render (g, p);
        }
        CHECK (isImageFilled (result, juce::Colours::white) == false);
    }

    SECTION ("InnerShadow 2px")
    {
        melatonin::InnerShadow shadow = { { juce::Colours::white, 2 } };

        {
            juce::Graphics g (result);
            g.fillAll (juce::Colours::white);

            g.fillAll (juce::Colours::white);
            g.setColour (juce::Colours::black);
            g.fillPath (p);

            // inner shadow render must come AFTER the path render
            shadow.render (g, p);
        }

        save_test_image (result, "inner_shadow_2px");

        SECTION ("outside of the path isn't touched (still white)")
        {
            REQUIRE (getPixel (result, 1, 1) == "FFFFFFFF");

            // left edge
            CHECK (getPixels (result, 1, { 2, 6 }) == "FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF");

            // top edge
            CHECK (getPixels (result, { 2, 6 }, 1) == "FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF");

            // right edge
            CHECK (getPixels (result, 7, { 2, 6 }) == "FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF");

            // bottom edge
            CHECK (getPixels (result, { 2, 6 }, 7) == "FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF");
        }

        SECTION ("path corners are blurred (no longer black or white)")
        {
            CHECK (getPixel (result, 2, 2) != "FF000000");
            CHECK (getPixel (result, 2, 2) != "FFFFFFFF");
            CHECK (getPixel (result, 2, 6) != "FF000000");
            CHECK (getPixel (result, 2, 6) != "FFFFFFFF");
            CHECK (getPixel (result, 6, 2) != "FF000000");
            CHECK (getPixel (result, 6, 2) != "FFFFFFFF");
            CHECK (getPixel (result, 6, 6) != "FF000000");
            CHECK (getPixel (result, 6, 6) != "FFFFFFFF");
        }

        SECTION ("center value (more than 2px from edge) is still black")
        {
            CHECK (getPixel (result, 4, 4) == "FF000000");
        }

        SECTION ("path corners are symmetrical")
        {
            // Note: Windows blurs seem off by exactly 1 integer of brightness, hence margin
            auto corner = result.getPixelAt (2, 2).getBrightness();
            CHECK (result.getPixelAt (2, 6).getBrightness() == Catch::Approx (corner).margin (0.01f));
            CHECK (result.getPixelAt (6, 2).getBrightness() == Catch::Approx (corner).margin (0.01f));
            CHECK (result.getPixelAt (6, 6).getBrightness() == Catch::Approx (corner).margin (0.01f));
        }

        SECTION ("With spread")
        {
            SECTION ("positive spread adds more blur")
            {
                auto centerWithoutSpread = result.getPixelAt (4, 4).getBrightness();
                auto cornerWithoutSpread = result.getPixelAt (2, 2).getBrightness();

                {
                    juce::Graphics g (result);
                    g.fillAll (juce::Colours::white);

                    // redo the blur with spread
                    melatonin::InnerShadow shadowWithPositiveSpread = { { juce::Colours::white, 2, {}, 1 } };
                    g.setColour (juce::Colours::black);
                    g.fillPath (p);

                    // inner shadow render must come AFTER the path render
                    shadowWithPositiveSpread.render (g, p);
                }
                save_test_image (result, "inner_shadow_2px_positive_spread");

                SECTION ("increases brightness at center")
                {
                    CHECK (result.getPixelAt (4, 4).getBrightness() > centerWithoutSpread);
                }

                SECTION ("corners actually go lighter as original image bleeds through more")
                {
                    CHECK (result.getPixelAt (2, 2).getBrightness() > cornerWithoutSpread);
                }

                SECTION ("center pixel is no longer black")
                {
                    CHECK (getPixel (result, 4, 4) != "FF000000");
                }
            }

            SECTION ("When there's more spread than radius")
            {
                // redo the blur with spread
                {
                    juce::Graphics g (result);
                    g.fillAll (juce::Colours::white);

                    g.fillAll (juce::Colours::white);
                    melatonin::InnerShadow shadowWithPositiveSpread = { { juce::Colours::white, 1, {}, 2 } };
                    g.setColour (juce::Colours::black);
                    g.fillPath (p);

                    shadowWithPositiveSpread.render (g, p);
                }
                save_test_image (result, "more spread than radius");

                SECTION ("edges of path are pure white")
                {
                    CHECK (getPixels (result, { 2, 6 }, 2) == "FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF");
                    CHECK (getPixels (result, 2, { 2, 6 }) == "FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF");
                    CHECK (getPixels (result, { 2, 6 }, 6) == "FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF");

                    // TODO: look into why windows is a bit off here...
                    // CHECK (getPixels (result, 6, { 2, 6 }) == "FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFFFFFF");
                }
            }
        }

        SECTION ("With offset")
        {
            SECTION ("x axis")
            {
                SECTION ("positive 2")
                {
                    {
                        juce::Graphics g (result);
                        g.fillAll (juce::Colours::white);
                        g.fillAll (juce::Colours::white);
                        shadow = { { juce::Colours::white, 2, { 2, 0 } } };
                        g.setColour (juce::Colours::black);
                        g.fillPath (p);

                        // inner shadow render must come AFTER the path render
                        shadow.render (g, p);
                    }

                    save_test_image (result, "inner_shadow_2px_positive_offset_x");

                    SECTION ("left edge is brightest")
                    {
                        for (auto i = 2; i < 7; ++i)
                        {
                            // remember, our inner shadow is cropped to the path bounds
                            // so 2,2 is the leftmost pixel of the image we care about
                            CHECK (result.getPixelAt (2, i).getBrightness() > result.getPixelAt (3, i).getBrightness());
                        }
                    }

                    SECTION ("rightmost center pixel is still black")
                    {
                        CHECK (getPixel (result, 6, 4) == "FF000000");
                    }
                }

                SECTION ("offset greater than radius matches figma/css")
                {
                    {
                        juce::Graphics g (result);
                        g.fillAll (juce::Colours::white);
                        g.fillAll (juce::Colours::white);
                        // offset larger than radius, so we can't pull pixels from the single chan blur anymore
                        shadow = { { juce::Colours::white, 2, { 3, 0 } } };
                        g.setColour (juce::Colours::black);
                        g.fillPath (p);

                        // inner shadow render must come AFTER the path render
                        shadow.render (g, p);
                    }

                    save_test_image (result, "inner_shadow_2px_positive_offset_3");

                    SECTION ("left edge is pure white")
                    {
                        for (auto i = 2; i < 7; ++i)
                        {
                            // remember, our inner shadow is cropped to the path bounds
                            // so 2,2 is the leftmost pixel of the image we care about
                            CHECK (result.getPixelAt (2, i).toDisplayString (true) == "FFFFFFFF");
                        }
                    }
                }

                SECTION ("negative")
                {
                    {
                        juce::Graphics g (result);
                        g.fillAll (juce::Colours::white);
                        g.fillAll (juce::Colours::white);
                        shadow = { { juce::Colours::white, 2, { -2, 0 } } };
                        g.setColour (juce::Colours::black);
                        g.fillPath (p);

                        // inner shadow render must come AFTER the path render
                        shadow.render (g, p);
                    }
                    save_test_image (result, "inner_shadow_2px_negative_offset_x");
                    // inner right edge has more white

                    // inner left 2px is black
                }
            }
        }

        // TODO: This isn't quite there.
        // Figma seems darker
        // Need to check CSS
        SECTION ("Figma compatibility")
        {
            //            auto fixtures = juce::File (PROJECT_ROOT).getChildFile ("tests/fixtures/figma");
            //            auto expected = fixtures.getChildFile ("5x5blackin9x9white2px-inner@1x.png");
            //            auto expectedImage = juce::ImageCache::getFromFile (expected);
            //
            //            CHECK (result.getWidth() == expectedImage.getWidth());
            //            CHECK (result.getHeight() == expectedImage.getHeight());
            //
            //            // check each pixel
            //            for (auto x = 0; x < result.getWidth(); ++x)
            //            {
            //                for (auto y = 0; y < result.getHeight(); ++y)
            //                {
            //                    auto actualPixel = result.getPixelAt (x, y).toString();
            //                    auto expectedPixel = expectedImage.getPixelAt (x, y).toString();
            //
            //                    // CHECK (actualPixel == expectedPixel);
            //                }
            //            }
        }
    }
    SECTION ("clips (to path, not path bounds)")
    {
        juce::Path rounded;

        // just show us one rounded corner
        // corners of 8 get us a very round corner
        rounded.addRoundedRectangle (-5, -5, 10, 10, 5);
        melatonin::InnerShadow shadow = { { juce::Colours::white, 2 } };

        float cornerColorWithoutShadow;

        {
            juce::Graphics g (result);
            g.fillAll (juce::Colours::white);
            g.setColour (juce::Colours::black);
            g.fillPath (rounded);
        }
        cornerColorWithoutShadow = result.getPixelAt (4, 4).getBrightness();
        CHECK (cornerColorWithoutShadow == Catch::Approx (1.0f));

        {
            juce::Graphics g (result);

            // inner shadow render must come AFTER the path render
            shadow.render (g, rounded);
        }

        SECTION ("corner still white")
        {
            CHECK (cornerColorWithoutShadow == Catch::Approx (1.0f));
        }
    }
}
