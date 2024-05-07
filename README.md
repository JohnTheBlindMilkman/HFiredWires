# HFiredWires

## Prequisites

This class can be used only on DST files which "know" the HMdcSeg class (from HYDRA).

## Usage

Two steps are required before one can get the desired wires collection:
1. Declare the HFiredWires object before the event loop, a.k.a. invoke the default constructor.
2. Invoke the default constructor method at each iteration of the track loop (before trying to use/get the wire collection).

Visual representation of what I'm writing:
```
void YourAnalysisMacro()
{
    HFiredWires firedWires;

    for (/*event loop*/)
    {
        for (/*track loop*/)
        {
            particle_cand = HCategoryManager::getObject(...);

            //some code
            
            firedWires = HFiredWires(particle_cand);
            auto wires = firedWires.GetWires();
        }
    }
}
```

## Doxygen support

Yes, everything is set up. If, for some unknown reason, you want a documentation for a single header, single class file you can have it. Just run the command `doxygen Doxyfile`.

## FAQ

### 1. Why did you create this doxygen? It isn't really necessary.

I have no idea.