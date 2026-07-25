import {
  PieChart,
  Pie,
  Cell,
  Tooltip,
  ResponsiveContainer,
  BarChart,
  Bar,
  XAxis,
  YAxis,
  CartesianGrid,
} from "recharts";

const COLORS = ["#4CAF50", "#F44336"];

export default function Charts({ data }) {
  if (!data) return null;

  const pieData = [
    {
      name: "Hits",
      value: data.statistics.hits,
    },
    {
      name: "Misses",
      value: data.statistics.misses,
    },
  ];

  const barData = data.accesses
    .slice(0, 30)
    .map((a) => ({
      address: a.address,
      cycles: a.cycles,
    }));

  return (
    <div className="charts">

      <div className="chart-card">
        <h2>Hit vs Miss</h2>

        <ResponsiveContainer width="100%" height={300}>
          <PieChart>
            <Pie
              data={pieData}
              dataKey="value"
              nameKey="name"
              outerRadius={90}
              label
            >
              {pieData.map((entry, index) => (
                <Cell
                  key={index}
                  fill={COLORS[index]}
                />
              ))}
            </Pie>

            <Tooltip />
          </PieChart>
        </ResponsiveContainer>
      </div>

      <div className="chart-card">
        <h2>Cycles (First 30 Accesses)</h2>

        <ResponsiveContainer width="100%" height={300}>
          <BarChart data={barData}>
            <CartesianGrid stroke="#444" />

            <XAxis dataKey="address" hide />

            <YAxis />

            <Tooltip />

            <Bar
              dataKey="cycles"
              fill="#4CAF50"
            />
          </BarChart>
        </ResponsiveContainer>
      </div>

    </div>
  );
}