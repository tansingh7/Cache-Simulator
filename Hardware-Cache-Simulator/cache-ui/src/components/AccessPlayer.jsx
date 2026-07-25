import { useEffect, useState } from "react";

function AccessPlayer({ accesses = [], current, setCurrent }) {
  const [index, setIndex] = useState(0);
  const [playing, setPlaying] = useState(false);

  useEffect(() => {
    setIndex(0);
    setPlaying(false);
  }, [accesses]);

  useEffect(() => {
    if (!playing || accesses.length === 0) return;

    const timer = setInterval(() => {
      setIndex((prev) => {
        if (prev >= accesses.length - 1) {
          setPlaying(false);
          return prev;
        }

        return prev + 1;
      });
    }, 400);

    return () => clearInterval(timer);
  }, [playing, accesses]);

  useEffect(() => {
    if (!setCurrent) return;

    setCurrent(accesses[index] || null);
  }, [accesses, index, setCurrent]);

  if (!accesses || accesses.length === 0) return null;

  const selected = current || accesses[index];

  return (
    <div className="player">
      <h2>Live Cache Access</h2>

      <div className="player-card">
        <h3>{selected.address}</h3>

        <p>
          <strong>Value:</strong> {selected.value}
        </p>

        <p>
          <strong>Cycles:</strong> {selected.cycles}
        </p>

        <p>
          <strong>Status:</strong>{" "}
          <span
            className={
              selected.status === "HIT"
                ? "hit"
                : "miss"
            }
          >
            {selected.status}
          </span>
        </p>
      </div>

      <div className="controls">
        <button
          onClick={() => setIndex(Math.max(index - 1, 0))}
          disabled={index === 0}
        >
          ◀
        </button>

        <button
          onClick={() => setPlaying((prev) => !prev)}
        >
          {playing ? "Pause" : "Play"}
        </button>

        <button
          onClick={() =>
            setIndex(Math.min(index + 1, accesses.length - 1))
          }
          disabled={index === accesses.length - 1}
        >
          ▶
        </button>
      </div>

      <p>
        Step {index + 1} / {accesses.length}
      </p>
    </div>
  );
}

export default AccessPlayer;
