export default function SearchBar({ value, setValue }) {

    return (

        <input
            className="search"
            placeholder="Search address..."
            value={value}
            onChange={(e)=>setValue(e.target.value)}
        />

    );

}