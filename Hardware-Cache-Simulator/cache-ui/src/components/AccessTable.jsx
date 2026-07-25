export default function AccessTable({ accesses }) {

    return (

<table className="table">

<thead>

<tr>

<th>Address</th>
<th>Value</th>
<th>Status</th>
<th>Cycles</th>

</tr>

</thead>

<tbody>

{accesses.map((a,index)=>(

<tr key={index}
className={a.status==="HIT"?"hit":"miss"}>

<td>{a.address}</td>

<td>{a.value}</td>

<td>{a.status==="HIT"?"🟢 HIT":"🔴 MISS"}</td>

<td>{a.cycles}</td>

</tr>

))}

</tbody>

</table>

    );

}