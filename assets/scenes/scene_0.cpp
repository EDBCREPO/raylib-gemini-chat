#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace rl { namespace scene {

    void scene_0( ptr_t<Scene> self ) {

        struct NODE {
            string_t Message = string_t( UNBFF_SIZE, '\0' );
            string_t Response, FullMessage;
        };  ptr_t<NODE> obj = new NODE();

    /*.........................................................................*/

        event_t<string_t> onGPT; onGPT.on([=]( string_t message ){
            
            fetch_t args; ssl_t ssl; args.method = "POST";
                    args.url    = regex::format("${0}?key=${1}",process::env::get("URL"),process::env::get("API"));

                    args.body   = json::stringify( object_t({
                        { "contents", array_t<object_t>({ object_t({
                            { "parts", array_t<object_t>({ object_t({
                                { "text", message }
                            }) }) }
                        }) }) }
                    }) );
                    
                    args.headers= header_t({
                        { "Content-Type", path::mimetype(".json") },
                        { "Host", url::hostname( args.url ) }
                    });

            https::fetch( args, &ssl )

            .then([=]( https_t cli ){
                try {
                auto raw = stream::await( cli ); // console::log( raw );
                auto req = json::parse( regex::replace_all( raw, "\\\\\"|*", "" ) );
                auto data= req["candidates"][0]["content"]["parts"][0]["text"].as<string_t>();
                     data= regex::replace_all( data, "\\\\n", "\n" ); 
                     obj->FullMessage = data; obj->Response.clear();
                } catch(...) { 
                    obj->FullMessage = "Something Went Wrong"; 
                    obj->Response.clear(); 
                }
            })

            .fail([=]( except_t err ){ console::log( err ); });

        });

    /*.........................................................................*/

        self->onDraw([=](){

            float w = GetRenderWidth(), h = GetRenderHeight(); ClearBackground( RAYWHITE );

            GuiDrawText( obj->Response.get(), { 2*w/100, 2*h/100, 96*w/100, 95*h/100 }, 0, BLACK ); 

            GuiTextBox( { 0, 95*h/100, 88*w/100, 5*h/100 }, obj->Message.get(), obj->Message.size(), true );

            if( GuiButton( { 88*w/100, 95*h/100, 12*w/100, 5*h/100 }, "Send" ) ){
                onGPT.emit( obj->Message.copy() );  obj->Message.fill( '\0' );
            }

        });

    /*.........................................................................*/

        self->onLoop([=]( float delta ){ [=](){
            static uint x = 0; static ulong y = 0;
        coStart

            while( y != obj->FullMessage.size() && !obj->FullMessage.empty() ){
               if( obj->FullMessage[ obj->Response.size() ]=='\n' ){ x=0; }
               if( x >= 149 ){ obj->Response.push('\n'); x=0; } 
                obj->Response.push( obj->FullMessage[y] );
                x++; y++; coNext;
            }   x=0; y=0;

            while( !obj->Response.empty() ){ coNext; }
            
        coStop
        }(); });

    }

}}

/*────────────────────────────────────────────────────────────────────────────*/
