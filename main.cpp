#include <rlfw/entry_point.hpp>
#include <rlfw/log.hpp>
#include <rlfw/BatchRenderer.hpp>
#include <rlfw/SpriteBatcher.hpp>
#include <rlfw/SpriteAtlasTexture.hpp>
#include <rlfw/shape_conversion.hpp>
#include <rlfw/export_png.hpp>

class MyApp final : public rlfw::App
{
    public:
        rlfw::BatchRenderer renderer{};
        rlfw::SpriteBatcher batcher{};
        rlfw::SpriteAtlasTexture atlas{};

        void onStart() override
        {
            renderer.Attach(batcher);
            rlfw::sprite_atlas::create_info info{};
            info.max_page_width = 512;
            info.max_page_height = 512;
            info.items =
            {
                rlfw::sprite_atlas::create_info::file{"aerie.png"},
                rlfw::sprite_atlas::create_info::file{"argosy.png"},
                rlfw::sprite_atlas::create_info::file{"bastion.png"},
            };
            rlfw::sprite_atlas atlas_data(info);
            rlfw::export_png(atlas_data.image.GetView(), ".", "atlas_page_{}.png");
            atlas.Load(atlas_data);
        }

        rlfw::f64 last_fps;
        void onUpdate(rlfw::f64 delta_seconds) override
        {
            auto new_fps = rlfw::fw::get_fps();
            if (last_fps != new_fps)
            {
                last_fps = new_fps;
                rlfw::info("fps: {}", new_fps);
            }

            batcher.Start(atlas);
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );    
        }
        void onConfigure(const rlfw::Args& args) override
        {
            rlfw::fw::set_window_title("MyApp");
            rlfw::fw::open_url("https://www.gnome.org/");
            rlfw::fw::crit("You have been gnomed!");
            rlfw::fw::force_close();
        }
        void onWindowMinimize() override
        {
                    batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.PushSprite(
                0,
                rlfw::as_quad2(
                    rlfw::box2<rlfw::f32>(
                        -1,
                        -1,
                        2,
                        2
                    )
                ),
                rlfw::color_rgba<rlfw::f32>(1, 1, 1, 1)
            );
            batcher.Finish();

            rlfw::fw::set_viewport();
            rlfw::fw::clear_color({0, 0, 0, 1});
            renderer.Load();
            renderer.Draw();
            renderer.Clear();
            rlfw::fw::swap_buffers();
        }

        void onKeyboardKey(rlfw::Scancode scancode, rlfw::ButtonState state) override
        {
            if (scancode == rlfw::Scancode::A && state == rlfw::ButtonState::Pressed)
            {
                rlfw::info("A!");
            }
        }

        void onMouseScroll(rlfw::vector2<rlfw::f32> translation) override
        {
           // rlfw::info("mouse scroll: ({}, {})", translation.x, translation.y);
        }

        void onMouseMove(rlfw::vector2<rlfw::i32> position, rlfw::vector2<rlfw::i32> translation) override
        {
           // rlfw::info("mouse move: ({}, {}) ({}, {})", position.x, position.y, translation.x, translation.y);
        }

        void onMouseButton(rlfw::MouseButton button, rlfw::ButtonState state) override
        {
            if (button == rlfw::MouseButton::Left && state == rlfw::ButtonState::Pressed)
            {
                rlfw::info("Left Pressed!");
            }
        }

        void onMouseHover(bool hover) override
        {
            //rlfw::info("hovering: {}", hover);
        }

        void onWindowMove(rlfw::vector2<rlfw::i32> position) override
        {
            //rlfw::info("window move: ({}, {})", position.x, position.y);
        }

        void onWindowResize(rlfw::vector2<rlfw::i32> size) override
        {
            //rlfw::info("window resize: ({}, {})", size.x, size.y);
        }

        void onWindowRestore() override
        {
            rlfw::info("window restore");
        }

        void onWindowMaximize() override
        {
            rlfw::info("window maximize");
        }

        void onFileDrop(std::filesystem::path& path) override
        {
            rlfw::info("file dropped: {}", path.c_str());
        }

        void onStop() override
        {

        }

        bool onClose() override
        {
            return true;
        }
};

RLFW_CREATE_ENTRY_POINT(MyApp)