package main

import (
	"encoding/binary"
	"flag"
	"io"
	"log"
	"os"
)

const (
	NumChannels   int16 = 1    //AUDIO_CHANNELS_PER_FRAME;
	BitsPerSample int16 = 32   //AUDIO_BITS_PER_CHANNEL;
	SamplingRate  int   = 8000 //AUDIO_SAMPLE_RATE;
	chunkSize     int   = 16
	audioFormat   int16 = 1
)

var (
	numOfSamples int
	ByteRate     int
	BlockAlign   int16
	DataSize     int
	totalSize    int
)

func main() {
	pcmFileName := flag.String("pcm", "input.pcm", "Input file in raw PCM format")
	wavFileName := flag.String("wav", "output.wav", "Output file (wav format)")
	flag.Parse()
	log.Printf("Convert file %s to file %s\n", *pcmFileName, *wavFileName)

	// read raw PCM data - 8 kHz, 32 bit BigEndian, 1 channel
	pcmData, err := os.ReadFile(*pcmFileName)
	if err != nil {
		log.Fatal(err)
	}

	numOfSamples = len(pcmData) / 4
	ByteRate = int(NumChannels) * int(BitsPerSample) * SamplingRate / 8
	BlockAlign = NumChannels * BitsPerSample / 8
	DataSize = int(NumChannels) * numOfSamples * int(BitsPerSample) / 8
	totalSize = 46 + DataSize

	// Output file.
	wavFile, err := os.Create(*wavFileName)
	if err != nil {
		log.Fatal(err)
	}
	defer wavFile.Close()

	wavFile.WriteString("RIFF")
	writeInt(wavFile, totalSize)
	wavFile.WriteString("WAVEfmt ")
	writeInt(wavFile, chunkSize)
	writeInt16(wavFile, audioFormat)
	writeInt16(wavFile, NumChannels)
	writeInt(wavFile, SamplingRate)
	writeInt(wavFile, ByteRate)
	writeInt16(wavFile, BlockAlign)
	writeInt16(wavFile, BitsPerSample)
	wavFile.WriteString("data")
	writeInt(wavFile, DataSize)
	wavFile.Write(toLE_int32(pcmData))

	if err := wavFile.Close(); err != nil {
		log.Fatal(err)
	}
}

func toLE_int32(be []byte) []byte {
	le := make([]byte, len(be))

	i := 0
	for i < len(le)-4 {
		le[i] = be[i+3]
		le[i+1] = be[i+2]
		le[i+2] = be[i+1]
		le[i+3] = be[i]
		i += 4
	}

	return le
}

func writeInt(w io.Writer, data int) {
	bs := make([]byte, 4)
	binary.LittleEndian.PutUint32(bs, uint32(data))
	w.Write(bs)
}

func writeInt16(w io.Writer, data int16) {
	bs := make([]byte, 2)
	binary.LittleEndian.PutUint16(bs, uint16(data))
	w.Write(bs)
}
