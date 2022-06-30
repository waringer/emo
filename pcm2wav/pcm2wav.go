package main

import (
	"encoding/binary"
	"flag"
	"io"
	"log"
	"os"
)

const (
	NumChannels  int16 = 1    //AUDIO_CHANNELS_PER_FRAME;
	SamplingRate int   = 8000 //AUDIO_SAMPLE_RATE;
	chunkSize    int   = 16
	audioFormat  int16 = 1
)

var (
	numOfSamples  int
	ByteRate      int
	BlockAlign    int16
	DataSize      int
	totalSize     int
	BitsPerSample int16 = 32 //AUDIO_BITS_PER_CHANNEL;
)

func main() {
	// parse flags
	use16bit := flag.Bool("16", false, "use 16bit wav format")
	inPCMFile := flag.String("pcm", "audio.pcm", "input PCM file to convert 8 kHz, 32 bit BigEndian, 1 channel")
	outWAVFile := flag.String("wav", "output.wav", "output WAV file")
	flag.Parse()
	log.Println("use16bit: ", *use16bit)
	log.Println("PCM File: ", *inPCMFile)
	log.Println("WAV File: ", *outWAVFile)

	// read raw PCM data - 8 kHz, 32 bit BigEndian, 1 channel
	pcmData, err := os.ReadFile(*inPCMFile)
	if err != nil {
		log.Fatal(err)
	}

	if *use16bit {
		BitsPerSample = 16
	}

	numOfSamples = len(pcmData) / 4
	ByteRate = int(NumChannels) * int(BitsPerSample) * SamplingRate / 8
	BlockAlign = NumChannels * BitsPerSample / 8
	DataSize = int(NumChannels) * numOfSamples * int(BitsPerSample) / 8
	totalSize = 46 + DataSize

	// Output file.
	wavFile, err := os.Create(*outWAVFile)
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
	if *use16bit {
		wavFile.Write(toLE_int16(pcmData))
	} else {
		wavFile.Write(toLE_int32(pcmData))
	}

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

func toLE_int16(be []byte) []byte {
	le := make([]byte, len(be)/2)

	bi, li := 0, 0
	for bi < len(be)-4 {
		beI := binary.BigEndian.Uint32(be[bi : bi+4])
		binary.LittleEndian.PutUint16(le[li:], uint16(beI>>16))

		bi += 4
		li += 2
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
