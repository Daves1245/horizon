# given an audio source, embed the raw PCM needed for the ac97 drawer
# directly into the binary. this is necessary for now until we have a
# proper VFS that can handle the data as files.
function(embed_audio name)
    # A for audio
    cmake_parse_arguments(A "" "SOURCE;SECTION" "" ${ARGN})

    if(A_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "embed_audio ${name}: stray arguments (mispelled arguments?): ${A_UNPARSED_ARGUMENTS}")
    endif()

    if(NOT A_SOURCE OR NOT A_SECTION)
        message(FATAL_ERROR "embed_audio ${name}: SOURCE and SECTION are required")
    endif()

    if(NOT AUDIO_OBJCOPY_FORMAT OR NOT AUDIO_OBJCOPY_ARCH)
        message(FATAL_ERROR "embed_audio ${name}: no objcopy output format for ARCH=${ARCH}")
    endif()

    if(NOT FFMPEG_BINARY)
        message(FATAL_ERROR "embed_audio ${name}: FFMPEG_BINARY is unset")
    endif()

    if(NOT TARGET horizon.kernel)
        message(FATAL_ERROR "embed_audio ${name}: horizon.kernel not defined yet")
    endif()

    get_filename_component(src ${A_SOURCE} ABSOLUTE BASE_DIR ${CMAKE_SOURCE_DIR})
    set(raw ${CMAKE_CURRENT_BINARY_DIR}/${name}.raw)
    set(obj ${CMAKE_CURRENT_BINARY_DIR}/${name}.o)

    # -f s16le / -acodec pcm_s16le: raw signed 16-bit little-endian PCM
    # -ar 48000: 48kHz sample rate (AC97 standard)
    # -ac 2: stereo
    add_custom_command(
        OUTPUT ${raw}
        COMMAND ${FFMPEG_BINARY} -y -i ${src} -f s16le -acodec pcm_s16le -ar 48000 -ac 2 ${raw}
        DEPENDS ${src}
        COMMENT "Converting ${name} to raw PCM"
    )

    add_custom_command(
        OUTPUT ${obj}
        COMMAND ${TOOLCHAIN_PREFIX}-objcopy
        -I binary -O ${AUDIO_OBJCOPY_FORMAT} -B ${AUDIO_OBJCOPY_ARCH}
        --rename-section .data=${A_SECTION},alloc,load,readonly,contents
        ${raw} ${obj}
        DEPENDS ${raw}
        COMMENT "Embedding ${name}.raw into ${A_SECTION}"
    )

    add_custom_target(${name}_obj DEPENDS ${obj})
    add_dependencies(horizon.kernel ${name}_obj)
    target_link_libraries(horizon.kernel PRIVATE ${obj})
endfunction()
